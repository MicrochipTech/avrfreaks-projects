/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file dcpu.cpp
    \brief Portable DCPU-16 CPU emulator

    The DCPU-16 is the in-game CPU used in the upcoming game 0x10^c, from the
    creators of the wildly successful Minecraft.  While the DCPU is supposed
    to be part of the game, it has serious potential for use in all sorts of
    embedded applications.
    
    The fact that DCPU is a very lightweight VM to implement and contains
    built-in instructions for accessing hardware peripheras and handling 
    external interrupts lends itself to being used on microcontrollers.  
    
    Unlike a lot of embedded CPUs, DCPU-16 assembly is extremely simple to 
    learn, since it has a very limited number of opcodes (37), each of which
    provide the same register/memory addressing modes for all operands.  
    There are also only 2 opcode formats which make interpreting opcodes very
    efficient.
    
    The DCPU-16 is extended using a variable number of "external hardware 
    devices" which communicate with the CPU core using interrupts.  These 
    devices are enumerated on startup, and since there is no defined format
    for how these devices work, we can hijack this interface to provide a 
    way for the DCPU to access resources supplied by the OS (i.e Timers, 
    Drivers), or the hardware directly.  This also lends itself to inter-VM
    communications (multiple DCPUs communicating with eachother in different
    OS threads).  There's an immense amount of flexibility here - applications
    from debugging to scripting to runtime-configuration are all easily 
    supported by this machine.
    
    But what is a platform without tools support?  Fortunately, the hype around
    0x10c is building - and a development community for this platform has 
    grown immensely.  There are a number of compilers, assemblers, and IDEs,
    many of which support virtualized hardware extensions.  One of the 
    compilers is a CLANG/LLVM backend, which should allow for very good C
    language support.
    
    I had attempted to do something similar by creating a VM based on the 8051
    (see the Funk51 project on sourceforge), but that project was at least
    four times as large - and the tools support was very spotty.  There were
    C compilers, but there was a lot of shimming required to produce output
    that was suitable for the VM.  Also, the lack of a native host interface
    (interrupts, hardware bus enumerations, etc.) forced a non-standard
    approach to triggering native methods by writing commands to a reserved
    chunk of memory and writing to a special "trigger" address to invoke the
    native system.  Using a DCPU-16 based simulator addresses this in a nice,
    clean way by providing modern tools, and a VM infrastruture tailored to
    be interfaced with a host.
    
    Regarding this version of the DCPU emulator - it's very simple to use.  
    Program binaries are loaded into buffers in the host CPU's RAM, with
    the host also providing a separate buffer for DCPU RAM.  The size of the
    DCPU RAM buffer will contain both the RAM area, as well as the program
    stack, so care must be taken to ensure that the stack doesn't overflow.
    The DCPU specification allows for 64K words (128KB) of RAM and ROM each,
    but this implementation allows us to tailor the CPU for more efficient
    or minimal environments.

    In the future, this emulator will be extended to provide a mechanism to 
    allow programs to be run out of flash, EEPROM, or other interfaces via
    the Mark3 Drivers API.
    
    Once the program has been loaded into the host's address space, the 
    DCPU class can be initialized.
    
    \code
    // Use 16-bit words for 16-bit emulator.
    K_USHORT ausRAM[ RAM_SIZE ];
    K_USHORT ausROM[ ROM_SIZE ];
    {
        class DCPU clMyDCPU;
        
        // Read program code into ausROM buffer here
        
        // Initialize the DCPU emulator
        clMyDCPU.Init( ausROM, RAM_SIZE, ausROM, ROM_SIZE );
    }
    \endcode
    
    Once the emulator has been initialized, the VM can be run one opcode at a
    time, as in the following example.
    
    \code
    while(1)
    {
        clMyCPU.RunOpcode();
    }
    \endcode
    
    To inspect the contents of the VM's registers, call the GetRegisters() 
    method.  This is useful for printing the CPU state on a regular basis, or
    using the PC value to determine when to end execution, or to provide an 
    offset for disassembling the current opcode.  
    
    \code 
    DCPU_Registers *pstRegisters;
    pstRegisters = clMyCPU.GetRegisters();
    \endcode 

*/
#include "dcpu.h"
#include "kerneltypes.h"
#include "ll.h"

#define CORE_DEBUG 0

//---------------------------------------------------------------------------
#if CORE_DEBUG

  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>

  #define DBG_PRINT(...)        printf(__VA_ARGS__)
#else
  #define DBG_PRINT(...)
#endif

//---------------------------------------------------------------------------
/*!
    Define the number of cycles that each "basic" opcode takes to execute.
*/
static const K_UCHAR aucBasicOpcodeCycles[] = 
{
    0,        // OP_NON_BASIC = 0
    1,        // OP_SET
    2,        // OP_ADD
    2,        // OP_SUB
    2,        // OP_MUL
    2,        // OP_MLI
    3,        // OP_DIV
    3,        // OP_DVI, 
    3,        // OP_MOD, 
    3,        // OP_MDI, 
    1,        // OP_AND, 
    1,        // OP_BOR, 
    1,        // OP_XOR, 
    1,        // OP_SHR, 
    1,        // OP_ASR, 
    1,        // OP_SHL, 
    2,        // OP_IFB, 
    2,        // OP_IFC, 
    2,        // OP_IFE, 
    2,        // OP_IFN, 
    2,        // OP_IFG, 
    2,        // OP_IFA, 
    2,        // OP_IFL, 
    2,        // OP_IFU, 
    0,        // OP_18,     
    0,        // OP_19,    
    3,        // OP_ADX,    
    3,        // OP_SBX,    
    0,        // OP_1C,    
    0,        // OP_1D,    
    2,        // OP_STI,    
    2,        // OP_STD     
};

//---------------------------------------------------------------------------
/*!
    Define the number of cycles that each "extended" opcode takes to execute.
*/
static const K_UCHAR aucExtendedOpcodeCycles[] = 
{
    0,    // "RESERVED",
    3,    // "JSR",        
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    4,    // "INT",        
    1,    // "IAG",        
    1,    // "IAS",        
    3,    // "RFI",        
    2,    // "IAQ",        
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    2,    // "HWN",        
    4,    // "HWQ",        
    4,    // "HWI",        
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
    0,    // "UNDEFINED"
};

//---------------------------------------------------------------------------
void DCPU::SET()
{
    DBG_PRINT("SET\n");
    *b = *a;
}

//---------------------------------------------------------------------------
void DCPU::ADD()
{
    K_ULONG ulTemp;
    DBG_PRINT("ADD\n");
    
    ulTemp = (K_ULONG)*a + (K_ULONG)*b;    
    if (ulTemp >= 65536) 
    {
        m_stRegisters.EX = 0x0001;
    }
    else
    {
        m_stRegisters.EX = 0;
    }
    
    *b = *b + *a;
}

//---------------------------------------------------------------------------
void DCPU::SUB()
{
    K_LONG lTemp;
    DBG_PRINT("SUB\n");
    
    lTemp = (K_LONG)*b - (K_LONG)*a;    
    if (lTemp < 0) 
    {        
        m_stRegisters.EX = 0xFFFF;
    }
    else
    {
        m_stRegisters.EX = 0;
    }
    
    *b = *b - *a;
}

//---------------------------------------------------------------------------
void DCPU::MUL()
{
    K_ULONG ulTemp;
    
    DBG_PRINT("MUL\n");
    ulTemp = (((K_ULONG)*a * (K_ULONG)*b));    
    m_stRegisters.EX = (K_USHORT)(ulTemp >> 16);
    *b = (K_USHORT)(ulTemp & 0x0000FFFF);
}

//---------------------------------------------------------------------------
void DCPU::MLI()
{
    K_LONG lTemp;
    
    DBG_PRINT("MLI\n");
    lTemp = ((K_LONG)(*(K_SHORT*)a) * (K_LONG)(*(K_SHORT*)b));
    m_stRegisters.EX = (K_USHORT)(lTemp >> 16);
    *b = (K_USHORT)(lTemp & 0x0000FFFF);
}

//---------------------------------------------------------------------------
void DCPU::DIV()
{
    K_USHORT usTemp;
    
    DBG_PRINT("DIV\n");
    if (*a == 0)
    {
        *b = 0;
        m_stRegisters.EX = 0;
    }
    else
    {
        usTemp = (K_USHORT)((((K_ULONG)*b) << 16) / (K_ULONG)*a);        
        *b = *b / *a;
        m_stRegisters.EX = usTemp;
    }
}

//---------------------------------------------------------------------------
void DCPU::DVI()
{
    K_USHORT usTemp;
    
    DBG_PRINT("DVI\n");
    if (*a == 0)
    {
        *b = 0;
        m_stRegisters.EX = 0;
    }
    else
    {
        usTemp = (K_USHORT)((((K_LONG)*((K_SHORT*)b)) << 16) / (K_LONG)(*(K_SHORT*)a));        
        *b = (K_USHORT)(*(K_SHORT*)b / *(K_SHORT*)a);
        m_stRegisters.EX = usTemp;
        
    }
}

//---------------------------------------------------------------------------
void DCPU::MOD()
{
    DBG_PRINT("MOD\n");
    if (*a == 0)
    {
        *b = 0;
    }
    else
    {
        *b = *b % *a;
    }
}

//---------------------------------------------------------------------------
void DCPU::MDI()
{
    DBG_PRINT("MDI\n");
    if (*b == 0)
    {
        *a = 0;
    }
    else
    {
        *b = (K_USHORT)(*((K_SHORT*)b) % *((K_SHORT*)a));
    }
}

//---------------------------------------------------------------------------
void DCPU::AND()
{
    DBG_PRINT("AND\n");
    *b = *b & *a;
}

//---------------------------------------------------------------------------
void DCPU::BOR()
{
    DBG_PRINT("BOR\n");
    *b = *b | *a;
}

//---------------------------------------------------------------------------
void DCPU::XOR()
{
    DBG_PRINT("XOR\n");
    *b = *b ^ *a;
}

//---------------------------------------------------------------------------
void DCPU::SHR()
{
    K_USHORT usTemp = (K_USHORT)((((K_ULONG)*b) << 16) >> (K_ULONG)*a);
    
    DBG_PRINT("SHR\n");
    *b = *b >> *a;
    m_stRegisters.EX = usTemp;
}

//---------------------------------------------------------------------------
void DCPU::ASR()
{
    K_USHORT usTemp = (K_USHORT)((((K_LONG)*b) << 16) >> (K_LONG)*a);
    
    DBG_PRINT("ASR\n");
    *b = (K_USHORT)(*(K_SHORT*)b >> *(K_SHORT*)a);
    m_stRegisters.EX = usTemp;
}
//---------------------------------------------------------------------------
void DCPU::SHL()
{
    K_USHORT usTemp = (K_USHORT)((((K_ULONG)*b) << (K_ULONG)*a) >> 16);
    
    DBG_PRINT("SHL\n");
    *b = *b << *a;
    m_stRegisters.EX = usTemp;
}

//---------------------------------------------------------------------------
bool DCPU::IFB()
{
    DBG_PRINT("IFB\n");    
    if ((*b & *a) != 0)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFC()
{
    DBG_PRINT("IFC\n");
    if ((*b & *a) == 0)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFE()
{
    DBG_PRINT("IFE\n");
    if (*b == *a)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFN()
{
    DBG_PRINT("IFN\n");
    if (*b != *a)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFG()
{
    DBG_PRINT("IFG\n");
    if (*b > *a)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFA()
{
    DBG_PRINT("IFA\n");
    if (*((K_SHORT*)b) > *((K_SHORT*)a))
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFL()
{
    DBG_PRINT("IFL\n");
    if (*b < *a)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool DCPU::IFU()
{
    DBG_PRINT("IFU\n");
    if (*(K_SHORT*)b < *(K_SHORT*)a)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
void DCPU::ADX()
{
    K_ULONG ulTemp;
    DBG_PRINT("ADX\n");
    ulTemp = (K_ULONG)*b + (K_ULONG)*a + (K_ULONG)m_stRegisters.EX;
    if (ulTemp >= 0x10000)
    {
        m_stRegisters.EX = 1;
    }
    else
    {
        m_stRegisters.EX = 0;
    }
    
    *b = ((K_USHORT)(ulTemp & 0x0000FFFF));    
}

//---------------------------------------------------------------------------
void DCPU::SBX()
{
    K_LONG lTemp;
    DBG_PRINT("SBX\n");
    lTemp = (K_LONG)*b - (K_LONG)*a + (K_LONG)m_stRegisters.EX;
    if (lTemp < 0 )
    {
        m_stRegisters.EX = 0xFFFF;
    }
    else
    {
        m_stRegisters.EX = 0;
    }
    
    *b = ((K_USHORT)(lTemp & 0x0000FFFF));    
}

//---------------------------------------------------------------------------
void DCPU::STI()
{
    DBG_PRINT("STI\n");
    *b = *a;
    m_stRegisters.I++;
    m_stRegisters.J++;
}

//---------------------------------------------------------------------------
void DCPU::STD()
{
    DBG_PRINT("STD\n");
    *b = *a;
    m_stRegisters.I--;
    m_stRegisters.J--;
}

//---------------------------------------------------------------------------
void DCPU::JSR()
{
    DBG_PRINT("JSR 0x%04X\n", *a);
    m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.PC;
    m_stRegisters.PC = *a;
}

//---------------------------------------------------------------------------
void DCPU::INT()
{
    DBG_PRINT("INT\n");

    if (m_stRegisters.IA == 0)
    {
        // If IA is not set, return out.
        return;
    }
    
    // Either acknowledge the interrupt immediately, or queue it.
    if (m_bInterruptQueueing == false)
    {    
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.PC;
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.A;
        
        m_stRegisters.A = *a;
        m_stRegisters.PC = m_stRegisters.IA;    
        m_bInterruptQueueing = true;
    }
    else
    {
        // Add interrupt message to the queue
        m_ausInterruptQueue[ ++m_ucQueueLevel ] = *a;
    }
}

//---------------------------------------------------------------------------
void DCPU::ProcessInterruptQueue()
{
    // If there's an interrupt address specified, queueing is disabled, and 
    // the queue isn't empty
    if (m_stRegisters.IA && !m_bInterruptQueueing && m_ucQueueLevel)
    {
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.PC;
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.A;
        
        m_stRegisters.A = m_ausInterruptQueue[ m_ucQueueLevel-- ];
        m_stRegisters.PC = m_stRegisters.IA;    
        
        m_bInterruptQueueing = true;
    }
}


//---------------------------------------------------------------------------
void DCPU::IAG()
{
    DBG_PRINT("IAG\n");
    
    *a = m_stRegisters.IA;
}

//---------------------------------------------------------------------------
void DCPU::IAS()
{
    DBG_PRINT("IAS\n");
    
    m_stRegisters.IA = *a;
}

//---------------------------------------------------------------------------
void DCPU::RFI()
{
    DBG_PRINT("RFI\n");
    
    /*! Disables interrupt queueing, pop A from the stack, then pops PC from
        the stack.  By disabling interrupt Queueing, we're essentially 
        re-enabling interrupts. */
    m_bInterruptQueueing = false;

    m_stRegisters.A  = m_pusRAM[ ++m_stRegisters.SP ];
    m_stRegisters.PC = m_pusRAM[ ++m_stRegisters.SP ];
    
}

//---------------------------------------------------------------------------
void DCPU::IAQ()
{
    DBG_PRINT("IAQ\n");
    
    /*! Add an interrupt to the interrupt queue if non-zero, if a = 0 then
        interrupts will be triggered as normal */
        
    if (*a)
    {
        m_bInterruptQueueing = true;    /*! Interrupts queued */
    }
    else
    {
        m_bInterruptQueueing = false;    /*! Interrups triggered */
    }
}

//---------------------------------------------------------------------------
void DCPU::HWN()
{        
    LinkListNode *pclNode;
    
    DBG_PRINT("HWN\n");
    m_usTempA = 0;
    /*! Returns the number of connected hardware devices to "a" */
    pclNode = m_clPluginList.GetHead();
    while (pclNode)
    {
        m_usTempA++;
        pclNode = pclNode->GetNext();
    }
    
    *a = m_usTempA;
}

//---------------------------------------------------------------------------
void DCPU::HWQ()
{
    DBG_PRINT("HWQ\n");
    DCPUPlugin *pclPlugin;
    pclPlugin = (DCPUPlugin*)m_clPluginList.GetHead();
    
    while (pclPlugin)
    {
        if (pclPlugin->GetDeviceNumber() == *a)
        {
            pclPlugin->Enumerate(&m_stRegisters);
            break;
        }        
        pclPlugin = (DCPUPlugin*)pclPlugin->GetNext();
    }
}

//---------------------------------------------------------------------------
void DCPU::HWI()
{
    DBG_PRINT("HWI\n");
    
    DCPUPlugin *pclPlugin;
    pclPlugin = (DCPUPlugin*)m_clPluginList.GetHead();
    
    while (pclPlugin)
    {        
        if (pclPlugin->GetDeviceNumber() == *a)
        {
            pclPlugin->Interrupt(this);
            break;
        }
        pclPlugin = (DCPUPlugin*)pclPlugin->GetNext();
    }
}

//---------------------------------------------------------------------------
void DCPU::Init(     K_USHORT *pusRAM_, K_USHORT usRAMSize_,
                    const K_USHORT *pusROM_, K_USHORT usROMSize_ )
{
    m_stRegisters.PC = 0;
    m_stRegisters.SP = usRAMSize_ ;
    m_stRegisters.A = 0;
    m_stRegisters.B = 0;
    m_stRegisters.C = 0;
    m_stRegisters.X = 0;
    m_stRegisters.Y = 0;
    m_stRegisters.Z = 0;
    m_stRegisters.I = 0;
    m_stRegisters.J = 0;
    m_stRegisters.EX = 0;
    m_stRegisters.IA = 0;
    m_ulCycleCount = 0;
    
    m_pusROM = (K_USHORT*)pusROM_;
    m_usROMSize = usROMSize_;
    
    m_pusRAM = pusRAM_;
    m_usRAMSize = usRAMSize_;
}

//---------------------------------------------------------------------------
K_UCHAR DCPU::GetOperand( K_UCHAR ucOpType_, K_USHORT **pusResult_ )
{
    K_UCHAR ucRetVal = 0;
    switch (ucOpType_)
    {
        case ARG_A:    case ARG_B:    case ARG_C:    case ARG_X:    
        case ARG_Y:    case ARG_Z:    case ARG_I:    case ARG_J:
            *pusResult_ = &m_stRegisters.ausRegisters[ ucOpType_ - ARG_A ];
            break;
                
        case ARG_BRACKET_A:    case ARG_BRACKET_B:    case ARG_BRACKET_C:    case ARG_BRACKET_X:            
        case ARG_BRACKET_Y:    case ARG_BRACKET_Z:    case ARG_BRACKET_I:    case ARG_BRACKET_J:
            *pusResult_ = &m_pusRAM[ m_stRegisters.ausRegisters[ ucOpType_ - ARG_BRACKET_A ] ];
            break;
        
        case ARG_WORD_A: case ARG_WORD_B: case ARG_WORD_C: case ARG_WORD_X: 
        case ARG_WORD_Y: case ARG_WORD_Z: case ARG_WORD_I: case ARG_WORD_J:
        {
            K_USHORT usTemp = m_pusROM[ m_stRegisters.PC++ ];
            usTemp += m_stRegisters.ausRegisters[ ucOpType_ - ARG_WORD_A ];
            *pusResult_ = &m_pusRAM[ usTemp ];
            ucRetVal = 1;
        }
            break;
        case ARG_PUSH_POP_SP:
            if (*pusResult_ == a)
            {
                a = &m_pusRAM[ ++m_stRegisters.SP ];
            }
            else
            {
                b = &m_pusRAM[ m_stRegisters.SP-- ];
            }
            break;    
        case ARG_PEEK_SP:
            *pusResult_ = &m_pusRAM[ m_stRegisters.SP ];            
            break;
        case ARG_WORD_SP:
        {
            K_USHORT usTemp = m_pusROM[ ++m_stRegisters.PC ];
            usTemp += m_stRegisters.SP;
            *pusResult_ = &m_pusRAM[ usTemp ];
            ucRetVal++;
        }
            break;        
        case ARG_SP:            
            *pusResult_ = &(m_stRegisters.SP);
            break;
        case ARG_PC:            
            *pusResult_ = &(m_stRegisters.PC);
            break;
        case ARG_EX:            
            *pusResult_ = &(m_stRegisters.EX);
            break;
        case ARG_NEXT_WORD:
            *pusResult_ = &m_pusRAM[ m_pusROM[ m_stRegisters.PC++ ] ];
            ucRetVal++;
            break;
        case ARG_NEXT_LITERAL:
            *pusResult_ = &m_pusROM[ m_stRegisters.PC++ ];
            ucRetVal++;
            break;
            
        case ARG_LITERAL_0:
            *pusResult_ = &m_usTempA;
            m_usTempA = (K_USHORT)(-1);        
            break;
        case ARG_LITERAL_1:    case ARG_LITERAL_2:    case ARG_LITERAL_3:    case ARG_LITERAL_4:
        case ARG_LITERAL_5:    case ARG_LITERAL_6: case ARG_LITERAL_7:    case ARG_LITERAL_8:
        case ARG_LITERAL_9:    case ARG_LITERAL_A:    case ARG_LITERAL_B:    case ARG_LITERAL_C:
        case ARG_LITERAL_D:    case ARG_LITERAL_E:    case ARG_LITERAL_F:    case ARG_LITERAL_10:
        case ARG_LITERAL_11: case ARG_LITERAL_12: case ARG_LITERAL_13: case ARG_LITERAL_14:
        case ARG_LITERAL_15: case ARG_LITERAL_16: case ARG_LITERAL_17: case ARG_LITERAL_18:
        case ARG_LITERAL_19: case ARG_LITERAL_1A: case ARG_LITERAL_1B: case ARG_LITERAL_1C:
        case ARG_LITERAL_1D: case ARG_LITERAL_1E: case ARG_LITERAL_1F:
            *pusResult_ = &m_usTempA;
            m_usTempA = (K_USHORT)(ucOpType_ - ARG_LITERAL_1);    
            break;
        default:
            break;
    }
    return ucRetVal;
}

//---------------------------------------------------------------------------
void DCPU::RunOpcode()
{
    // Fetch the opcode @ the current program counter
    K_USHORT usWord = m_pusROM[ m_stRegisters.PC++ ];        
    K_UCHAR ucOp = (K_UCHAR)DCPU_NORMAL_OPCODE_MASK(usWord);
    K_UCHAR ucA = (K_UCHAR)DCPU_A_MASK(usWord);
    K_UCHAR ucB = (K_UCHAR)DCPU_B_MASK(usWord);
    K_UCHAR ucSize = 1;

    DBG_PRINT("0x%04X: %04X\n", m_stRegisters.PC - 1, usWord );

    // Decode the opcode 
    if (ucOp)
    {
        bool bRunNext = true;
        
        a = &m_usTempA;
        b = 0;
        
        // If this is a "basic" opcode, decode "a" and "b"
        ucSize += GetOperand( ucA , &a );
        ucSize += GetOperand( ucB, &b );

        // Add the cycles to the runtime clock
        m_ulCycleCount += (K_ULONG)aucBasicOpcodeCycles[ ucOp ];
        m_ulCycleCount += (ucSize - 1);
        
        // Execute the instruction once we've decoded the opcode and 
        // processed the arguments.
        switch (DCPU_NORMAL_OPCODE_MASK(usWord))
        {
            case OP_SET:    SET();        break;
            case OP_ADD:    ADD();        break;
            case OP_SUB:    SUB();        break;

            case OP_MUL:    MUL();        break;
            case OP_MLI:    MLI();        break;            
            case OP_DIV:    DIV();        break;
            case OP_DVI:    DVI();        break;
            case OP_MOD:    MOD();        break;
            case OP_MDI:    MDI();        break;
            case OP_AND:    AND();        break;
            case OP_BOR:    BOR();        break;
            case OP_XOR:    XOR();        break;
            case OP_SHR:    SHR();        break;
            case OP_ASR:    ASR();        break;
            case OP_SHL:    SHL();        break;
            case OP_IFB:    bRunNext = IFB();    break;            
            case OP_IFC:    bRunNext = IFC();    break;
            case OP_IFE:    bRunNext = IFE();    break;                
            case OP_IFN:    bRunNext = IFN();    break;
            case OP_IFG:    bRunNext = IFG();    break;            
            case OP_IFA:    bRunNext = IFA();    break;
            case OP_IFL:    bRunNext = IFL();    break;
            case OP_IFU:    bRunNext = IFU();    break;
            case OP_ADX:    ADX();        break;
            case OP_SBX:    SBX();        break;
            case OP_STI:    STI();        break;
            case OP_STD:    STD();        break;
            default:    break;
        }
        
        // If we're not supposed to run the next instruction (i.e. skip it 
        // due to failed condition), adjust the PC.
        if (!bRunNext)
        {            
            // Skipped branches take an extra cycle 
            m_ulCycleCount++;
            
            // Skip the next opcode
            usWord = m_pusROM[ m_stRegisters.PC++ ];            
            if (DCPU_NORMAL_OPCODE_MASK(usWord))
            {
                DBG_PRINT( "Skipping Basic Opcode: %X\n", DCPU_NORMAL_OPCODE_MASK(usWord));
                // If this is a "basic" opcode, decode "a" and "b" - we do this to make sure our
                // PC gets adjusted properly.
                GetOperand( DCPU_A_MASK(usWord), &a );
                GetOperand( DCPU_B_MASK(usWord), &b );
            }
            else
            {
                DBG_PRINT( "Skipping Extended Opcode: %X\n", DCPU_EXTENDED_OPCODE_MASK(usWord));
                GetOperand( DCPU_A_MASK(usWord), &a );
            }
        }
    }
    else
    {        
        // Extended opcode.  These only have a single argument, stored in the 
        // "a" field.
        GetOperand( ucA, &a );
        m_ulCycleCount++;

        // Execute the "extended" instruction now that the opcode has been
        // decoded, and the arguments processed.
        switch (ucB)
        {
            case OP_EX_JSR:        JSR();    break;
            case OP_EX_INT:        INT();    break;
            case OP_EX_IAG:        IAG();    break;
            case OP_EX_IAS:        IAS();    break;
            case OP_EX_RFI:        RFI();    break;
            case OP_EX_IAQ:        IAQ();    break;
            case OP_EX_HWN:        HWN();    break;
            case OP_EX_HWQ:        HWQ();    break;
            case OP_EX_HWI:        HWI();    break;
            default:    break;
        }
    }
    
    // Process an interrupt from the queue (if there is one)
    ProcessInterruptQueue();    
}

//---------------------------------------------------------------------------
void DCPU::SendInterrupt( K_USHORT usMessage_ )
{
    if (m_stRegisters.IA == 0)
    {
        // If IA is not set, return out.
        return;
    }
    
    // Either acknowledge the interrupt immediately, or queue it.
    if (m_bInterruptQueueing == false)
    {    
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.PC;
        m_pusRAM[ m_stRegisters.SP-- ] = m_stRegisters.A;
        
        m_stRegisters.A = usMessage_;
        m_stRegisters.PC = m_stRegisters.IA;    
        m_bInterruptQueueing = true;
    }
    else
    {
        // Add interrupt message to the queue
        m_ausInterruptQueue[ ++m_ucQueueLevel ] = usMessage_;
    }    
}

//---------------------------------------------------------------------------
void DCPU::AddPlugin( DCPUPlugin *pclPlugin_ )
{
    m_clPluginList.Add( (LinkListNode*)pclPlugin_ );    
}
