//==================================================================
//
//  Generic Header information generated by 13705CFG.EXE (Build 9)
//
//  Copyright (c) 2000,2001 Epson Research and Development, Inc.
//  All rights reserved.
//
//  Panel: 320x240x4bpp 53Hz Color 12-Bit TFT/D-TFD (PCLK=12.000MHz)
//
//  This file defines the configuration environment and registers,
//  which can be used by any software, such as display drivers.
//
//  PLEASE NOTE: If you FTP this file to a non-Windows platform, make
//               sure you transfer this file using ASCII, not BINARY
//               mode.
//
//==================================================================

#define S1D_DISPLAY_WIDTH               320
#define S1D_DISPLAY_HEIGHT              240
#define S1D_DISPLAY_BPP                 4
#define S1D_DISPLAY_SCANLINE_BYTES      160
#define S1D_DISPLAY_FRAME_RATE          53
#define S1D_DISPLAY_PCLK                12000000L
#define S1D_PHYSICAL_REG_ADDR           0x00000046L
#define S1D_PHYSICAL_VMEM_ADDR          0x00F00000L
#define S1D_PHYSICAL_REG_SIZE           31L
#define S1D_PHYSICAL_VMEM_SIZE          81920L
#define S1D_PALETTE_SIZE                256
#define S1D_FRAME_RATE                  53
#define S1D_POWER_DELAY_OFF             500
#define S1D_POWER_DELAY_ON              0
#define S1D_REGDELAYOFF                 0xFE
#define S1D_REGDELAYON                  0xFF

#define S1D_WRITE_PALETTE(p,i,r,g,b)  \
  {  \
  ((volatile S1D_VALUE*)(p))[0x15/sizeof(S1D_VALUE)] = (S1D_VALUE)(i); \
  ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)] = (S1D_VALUE)(r); \
  ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)] = (S1D_VALUE)(g); \
  ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)] = (S1D_VALUE)(b); \
  }

#define S1D_READ_PALETTE(p,i,r,g,b)   \
  {  \
  ((volatile S1D_VALUE*)(p))[0x15/sizeof(S1D_VALUE)] = (S1D_VALUE)(i); \
  r = ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)]; \
  g = ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)]; \
  b = ((volatile S1D_VALUE*)(p))[0x17/sizeof(S1D_VALUE)]; \
  }

typedef unsigned char S1D_INDEX;
typedef unsigned char S1D_VALUE;
typedef struct
{
    S1D_INDEX Index;
    S1D_VALUE Value;
} S1D_REGS;

static S1D_REGS aS1DRegs[] =
{
    { 0x00,             0x24 },       // Revision Code Register
    { 0x01,             0xA1 },       // Mode Register 0 Register
    { 0x02,             0xB0 },       // Mode Register 1 Register
    { 0x03,             0x07 },       // Mode Register 2 Register
    { 0x04,             0x27 },       // Horizontal Panel Size Register
    { 0x05,             0xEF },       // Vertical Panel Size Register (LSB)
    { 0x06,             0x00 },       // Vertical Panel Size Register (MSB)
    { 0x07,             0x05 },       // FPLINE Start Position Register
    { 0x08,             0x0C },       // Horizontal Non-Display Period Register
    { 0x09,             0x04 },       // FPFRAME Start Position Register
    { 0x0A,             0x0B },       // Vertical Non-Display Period Register
    { 0x0B,             0x00 },       // MOD Rate Register
    { 0x0C,             0x00 },       // Screen 1 Start Address Register (LSB)
    { 0x0D,             0x00 },       // Screen 1 Start Address Register (MSB)
    { 0x0E,             0x00 },       // Screen 2 Start Address Register (LSB)
    { 0x0F,             0x00 },       // Screen 2 Start Address Register (MSB)
    { 0x10,             0x00 },       // Screen Start Address Overflow Register
    { 0x11,             0x00 },       // Memory Address Offset Register
    { 0x12,             0xFF },       // Screen 1 Vertical Size Register (LSB)
    { 0x13,             0x03 },       // Screen 1 Vertical Size Register (MSB)
    { 0x14,             0x00 },       // Not Used
    { 0x15,             0x00 },       // Look-Up Table Address Register
    { 0x16,             0x00 },       // Not Used
    { 0x17,             0x00 },       // Look-Up Table Data Register
    { 0x18,             0x00 },       // GPIO Configuration Control Register
    { 0x19,             0x02 },       // GPIO Status/Control Register
    { 0x1A,             0x00 },       // Scratch Pad Register
    { 0x1B,             0x00 },       // SwivelView Mode Register
    { 0x1C,             0xA0 },       // Line Byte Count Register
    { 0x1D,             0x00 },       // Not Used
    { 0x1E,             0x00 },       // Not Used
    { 0x1F,             0x00 },       // Not Used
};
