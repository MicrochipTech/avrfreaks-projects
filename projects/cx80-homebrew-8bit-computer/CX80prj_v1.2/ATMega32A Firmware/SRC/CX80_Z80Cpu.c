/*
 * CX80_Z80CPU.c
 *
 * Created: 13/05/2013 11:44:26
 *  Author: Calogiuri Enzo Antonio
 */ 

#include "CX80.h"

/*Module for Z80 CPU emulation*/

#define ADDA		0
#define ADCA		1
#define SUB			2
#define SBCA		3
#define AND			4
#define XOR			5
#define OR			6
#define CP			7

#define RLC			0
#define RRC			1
#define RL			2
#define RR			3
#define SLA			4
#define SRA			5
#define SLL			6
#define SRL			7

byte reg[12], reg2[8];
byte fC, fN, fP, fU3, fH, fU5, fZ, fS;
byte fC_, fN_, fP_, fU3_, fH_, fU5_, fZ_, fS_;
word regPC, regSP;
byte CPU_Stop_Run;
byte ixy, icb;
signed char ofs;
char dreginmem;
long rl;

const byte pt[]={ 
         // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
			
/*=====================================================================*/

/*Setup the Z80 CPU*/
void CX80_InitZ80CPU(void)
{
	byte i;
	
	for (i = 0; i < 12; i++)
		reg[i] = 0;
		
	regPC = CX80_CPU_STARTRAM;
	regSP = 0x7FFF;
	ixy = 0;
}

/*Obtain a byte from ram*/
static byte Fetch(void)
{
	return RMem(regPC++);
}

/*Obtain a word from ram*/
static word Fetch16(void)
{
	register word r = RMem16(regPC);
	
	regPC += 2;
	
	return r;
}

/*Exchanges two byte*/
static void Swap(byte *a, byte *b)
{
	register byte t;
     
     t = *a;
     *a = *b;
     *b = t;
}

/*Swap flags*/
static void SwapFlags(void)
{
	Swap(&fC, &fC_);
    Swap(&fN, &fN_);
    Swap(&fP, &fP_);
    Swap(&fU3, &fU3_);
    Swap(&fH, &fH_);
    Swap(&fU5, &fU5_);
    Swap(&fZ, &fZ_);
    Swap(&fS, &fS_);
}

/*Return a flag status*/
static char gflag(byte i)
{
	i &= 7;
	
	if (i == FNZ)
		return !(fZ);
		
	if (i == FZ)
		return (fZ);
		
	if (i == FNC)
		return !(fC);
		
	if (i == FC)
		return (fC);
		
	if (i == FPO)
		return !(fP);
		
	if (i == FPE)
		return (fP);
		
	if (i == FP)
		return !(fS);
		
	if (i == FM)
		return (fS);
	
	return 0;
}

/*Set 16bit register*/
static void set16op(byte i, word v)
{
	register byte bh = v >> 8;
	register byte bl = v & 0xFF;
	
	switch (i & 3)
	{
		case	BC:
			reg[B] = bh;
			reg[C] = bl;
			break;
			
		case	DE:
			reg[D] = bh;
			reg[E] = bl;
			break;
			
		case	HL:
			reg[H + ixy] = bh;
			reg[L + ixy] = bl;
			break;
			
		case	rSP:
			regSP = v;
	}
}

/*get value from 16bit register*/
static word get16op(byte i)
{
	switch (i & 3)
	{
		case	BC:	return (word)(reg[B] << 8) + reg[C];
		case	DE: return (word)(reg[D] << 8) + reg[E];
		case	HL: return (word)(reg[H + ixy] << 8) + reg[L + ixy];
		case	rSP: return regSP;
		default	  : return 0;
	}
}

/*Obtain a value from registers or ram location*/
static word dreg(byte i)
{
	register word r;
	
	if (i == PHL)
	{
        dreginmem = 1;  
          
		if (ixy == 0)
			r = (word)((reg[H] << 8) + reg[L]);
		else
		{
			if (icb == 0)
				ofs = (signed char)Fetch();
				
			icb = 0;
			
			if (ixy == 4)
				r = (word)((reg[IXH] << 8) + reg[IXL] + ofs);
			else
				r = (word)((reg[IYH] << 8) + reg[IYL] + ofs);
			
			ixy = 0;
		}
	}
	else
		if ((i & 0xE) == H)
		{
			r = i + ixy;
			dreginmem = 0;
			ixy = 0;
		}
		else
		{
			r = i;
			dreginmem = 0;
		}
		
	return r;
}

/*Obtain a value from 8bit register*/
static byte get8op(byte i)
{
	register byte r;
	
	if (i == PHL)
	{
		if (ixy == 0)
			r = RMem( (word)(reg[H] << 8) + reg[L] );
		else
		{
			if (icb == 0)
				ofs = (signed char)Fetch();
				
			icb = 0;
			
			if (ixy == 4)
				r = RMem( (word)(reg[IXH] << 8) + reg[IXL] + ofs );
			else
				r = RMem( (word)(reg[IYH] << 8) + reg[IYL] + ofs );
				
			ixy = 0;
		}
	}
	else
		if ((i & 0xE) == H)
		{
			r = reg[i + ixy];
			ixy = 0;
		}
		else
			r = reg[i];
			
	return r;
}

/*Emulate the alu module into Z80 CPU*/
static void Alu(byte op, byte v)
{
	register int r = reg[A];
	
	switch(op)
	{
		case		ADDA:
			reg[A] = (byte)(r += v);
			fN = 0;
			fP = ((r > 127) || (r <-128));
			fC = ((r > 255) || (r < 0));
			break;
			
		case		ADCA:
			r += (v + fC);
			fN = 0;
			fP = ((r > 127) || (r <-128));
			fC = ((r > 255) || (r < 0));
			fH = ((reg[A] & 0xF) + ((v + fC) & 0xF) > 0xF);
			reg[A] = (byte)r;
			break;
			
		case		SUB:
			reg[A] = (byte)(r -= v);
			fN = 1;
			fP = ((r > 127) || (r <-128));
			fC = ((r > 255) || (r < 0));
			break;
			
		case		SBCA:
			reg[A] = (byte)(r -= (v + fC));
			fN = 1;
			fP = ((r > 127) || (r <-128));
			fC = ((r > 255) || (r < 0));
			break;
			
		case		AND:
			reg[A] = (r &= v);
			fN = 0;
			fH = 1;
			fC = 0;
			fP = (pt[r >> 4] + pt[r & 0xf]);
			break;
			
		case		XOR:
			reg[A] = (r ^= v);
			fC = 0;
			fN = 0;
			fH = 0;
			fP = (pt[r >> 4] + pt[r & 0xf]);
			break;
			
		case		OR:
			reg[A] = (r |= v);
			fC = 0;
			fN = 0;
			fH = 0;
			fP = (pt[r >> 4] + pt[r & 0xf]);
			break;
			
		case		CP:
			r -= v;
			fN = 1;
			fP = ((r > 127) || (r <-128));
			fC = ((r > 255) || (r < 0));
			break;
	}
	
	fS = ((r & 0x80) != 0);
	fZ = ((r & 0xFF) == 0);
}

/*Extract from stack*/
static word Pop(void)
{
	register word nn;
	
	nn = RMem(regSP++);
	nn += RMem(regSP++) << 8;
	
	return nn;
}

/*Insert into stack*/
static void Push(word nn)
{
	WMem(--regSP, nn >> 8);
	WMem(--regSP, nn & 0xFF);
}

/*Used after calling dreg, write relative value into ram or register*/
static void WriteFromdreg(word Addr, byte Val)
{
	if (dreginmem == 1)
		WMem(Addr, Val);
	else
		reg[Addr] = Val;
}

/*Used after callig dreg, read relative value into ram or register*/
static byte ReadFromdreg(word Addr)
{
	if (dreginmem == 1)
		return RMem(Addr);
	else
		return reg[Addr];
}

/*Emulate the Z80 CPU*/
void CX80_Z80Run(void)
{
	register byte x, y, z, Opcode, bt, bt2;
	register word nn, p1;
	register int r;	
	
	Opcode = Fetch();
	
	x = (Opcode & 0xC0) >> 6;
    y = (Opcode & 0x38) >> 3;
    z = (Opcode & 0x07);
	
	switch (x)
	{
		case				0:
			switch (z)
			{
				case				0:
					switch (y)
					{
						case				0: break;		// NOP
						
						case				1:// EX AF AF'
							bt = reg[A]; reg[A] = reg2[A]; reg2[A] = bt;
							
							SwapFlags();
							break;
							
						case				2: // DJNZ
							--reg[B];
							
							ofs = (signed char)Fetch();
							
							if (reg[B] != 0)
								regPC += ofs;
							
                            fS = ((reg[B] & 0x80) != 0);
                            fZ = (reg[B] == 0);
							break;
							
						case				3:// JR+d
							ofs = (signed char)Fetch();
							regPC += ofs;
							break;
							
						case				4:// JR NZ
							ofs = (signed char)Fetch();
							
							if (!fZ)
								regPC += ofs;
								
							break;
							
						case				5:// JR Z
							ofs = (signed char)Fetch();
							
							if (fZ)
								regPC += ofs;
								
							break;
							
						case				6:// JR NC
							ofs = (signed char)Fetch();
							
							if (!fC)
								regPC += ofs;
								
							break;
							
						case				7:// JR C
							ofs = (signed char)Fetch();
							
							if (fC)
								regPC += ofs;
								
							break;
					}
					break;
					
				case				1: // z=1 16-bit immediate ld and add
					if (y & 1)	// ADD HL, pair
					{
						rl = (word)((reg[H + ixy] << 8));
						rl += (long)(reg[L + ixy]);
						rl += (long)(get16op(y >> 1));
						
						//set16op(HL, (word)rl);
						reg[H + ixy] = (word)(rl >> 8);
			            reg[L + ixy] = ((word)(rl)) & 0xFF;
						
						fC = ((rl > 65535) || (rl < 0));
						fS = ((rl & 0x8000) != 0);
						fZ = ((rl & 0xFFFF) == 0);
					}
					else
						set16op(y >> 1, Fetch16());
						
					break;
					
				case				2:// z=2 indirect loading
					switch (y)
					{
						case			0:// LD (BC),A
							WMem((word)(reg[B] << 8) + reg[C], reg[A]);
							break;
							
						case			1:// LD A,(BC)
							reg[A] = RMem((word)(reg[B] << 8) + reg[C]);
							break;
							
						case			2:// LD (DE),A
							WMem((word)(reg[D] << 8) + reg[E], reg[A]);
							break;
							
						case			3:// LD A,(DE)
							reg[A] = RMem((word)(reg[D] << 8) + reg[E]);
							break;
							
						case			4:// LD (nn),HL
							nn = Fetch16();						
							
							WMem(nn++, reg[L + ixy]);							
							
							WMem(nn, reg[H + ixy]);
							
							ixy = 0;
							break;
							
						case			5:// LD HL,(nn)
							nn = Fetch16();							
							
							reg[L + ixy] = RMem(nn++);							
							
							reg[H + ixy] = RMem(nn);
							
							ixy = 0;
							break;
							
						case			6:// LD (nn),A
							nn = Fetch16();
							
							WMem(nn, reg[A]);
							break;
							
						case			7:// LD A,(nn)
							nn = Fetch16();
							
							reg[A] = RMem(nn);
							break;
					}
					break;
					
				case				3:// z=3 16-bit INC and DEC
					if (y & 1)
					{
						nn = get16op(y >> 1) - 1;
						set16op(y >> 1, nn);
					}
					else
					{
						nn = get16op(y >> 1) + 1;
						set16op(y >> 1, nn);
					}
					break;
					
				case				4: // z=4 8-bit INC
					nn = dreg(y);
					
					if (dreginmem == 1)
					{
						bt = (byte)(RMem(nn) + 1);
						
						WMem(nn, bt);
					}
					else
						bt = reg[(byte)nn] += 1;
						
					fS = ((bt & 0x80) != 0);
					fZ = (bt == 0);
					fP = (pt[bt >> 4] + pt[bt & 0xf]);
					fN = 0;
					fH = ((((bt - 1) & 0xF) + 1) & 16) != 0;
					break;
					
				case				5:// z=5 8-bit DEC
					nn = dreg(y);
					
					if (dreginmem == 1)
					{
						bt = (byte)(RMem(nn) - 1);
						
						WMem(nn, bt);
					}
					else
						bt = reg[nn] -= 1;
						
					fS = ((bt & 0x80) != 0);
					fZ = (bt == 0);
					fP = (pt[bt >> 4] + pt[bt & 0xf]);
					fN = 1;
					fH = ((((bt + 1) & 0xF) - 1) & 16) != 0;
					break;
					
				case				6:// z=6 LD r,N
					nn = dreg(y);
					
					if (dreginmem == 1)
						WMem(nn, Fetch());
					else
						reg[(byte)nn] = Fetch();
						
					break;
					
				case				7:// z=7 assorted op on acc and flags
					switch (y)
					{
						case			0:// RLCA 8-bit
							bt = reg[A];
							fC = (bt >> 7);
							reg[A] = (bt << 1) + (bt >> 7);
							
							fS = ((reg[A] & 0x80) != 0);
							fZ = (reg[A] == 0);
							fN = 0;
							fH = 0;
							break;
							
						case			1:// RRCA 8-bit
							bt = reg[A];
							fC = (bt & 1);
							reg[A] = (bt >> 1) + (bt << 7);
							fN = 0;
							fH = 0;
							break;
							
						case			2:// RLA 9-bit
							bt = reg[A];
							reg[A] = (reg[A] << 1) + fC;
							fC = (bt >> 7);
							fN = 0;
							fH = 0;
							break;
							
						case			3:// RRA 9-bit
							bt = reg[A];
							reg[A] = (bt >> 1) + (fC << 7);
							fC = (bt & 1);
							fN = 0;
							fH = 0;
							break;
							
						case			4:// DAA
							nn = reg[A];
							
							if (fN)
							{
								if (fH || (reg[A] & 0xF) > 9)
									nn -= 6;
									
								if (fC || nn > 0x99)
									nn -= 0x60;
							}
							else
							{
								if (fH || (reg[A] & 0xF) > 9)
									nn += 6;
									
								if (fC || reg[A] > 0x99)
									nn += 0x60;
							}
							
							fC = fC || (nn > 0x99) || (nn > 255);
							reg[A] = (byte)nn;
							fZ = (reg[A] == 0);
							fS = ((reg[A] & 0x80) != 0);
							fP = (pt[reg[A] >> 4] + pt[reg[A] & 0xf]);
							break;
							
						case			5:// CPL
							reg[A] ^= 0xFF;
							fN = 1;
							fH = 1;
							break;
							
						case			6:// SCF
							fC = 1;
							fN = 0;
							fH = 0;
							break;
							
						case			7:// CCF
							fC = 1 - fC;
							fH = fC;
							fN = 0;
							break;
					}
					break;
			}
			break;
			
		case					1:// 8-bit loading    LD r1,r2
			if (z == PHL && y == PHL)	//Halt
				while (1);   // wait for an reset
			else
			{
				if (y == PHL)
				{
					nn = dreg(y);
					bt = get8op(z);
				}
				else
				{
					bt = get8op(z);
					nn = dreg(y);
				}
				
				if (dreginmem == 0)
				   reg[(byte)nn]=bt;
	            else
	               WMem(nn, bt);
			}
			break;
			
		case					2:// arithmetic and logic with reg
			Alu(y, get8op(z));
			break;
			
		case					3:// miscellaneous group
			switch (z)
			{
				case			0: // RET cc
					if (gflag(y))
						regPC = Pop();
						
					break;
					
				case			1:// pop & various ops				
					switch (y)
					{
						case		0:// pop BC
							nn = Pop();
							reg[B] = nn >> 8;
							reg[C] = nn & 0xFF;
							break;
							
						case		1:// RET
							regPC = Pop();
							break;
							
						case		2:// pop DE
							nn = Pop();
							reg[D] = nn >> 8;
							reg[E] = nn & 0xFF;
							break;
							
						case		3:// EXX
							for (nn = 0; nn < 6; nn++)
							{
								bt = reg[nn];
								reg[nn] = reg2[nn];
								reg2[nn] = bt;
							}
							break;
							
						case		4:// pop HL
							nn = Pop();
							reg[H + ixy] = nn >> 8;
							reg[L + ixy] = nn & 0xFF;
							break;
							
						case		5:// JP HL
							regPC = (word)(reg[H + ixy] << 8) + reg[L + ixy];
							break;
							
						case		6:// pop AF
							nn = Pop();
							reg[A] = nn >> 8;
							fC = nn&1;  nn>>=1;
							fN = nn&1;  nn>>=1;
							fP = nn&1;  nn>>=1;
							fU3 = nn&1;  nn>>=1;
							fH = nn&1;  nn>>=1;
							fU5 = nn&1;  nn>>=1;
							fZ = nn&1;  nn>>=1;
							fS = nn&1;  nn>>=1;
							break;
							
						case		7:// LD SP, HL
							regSP = (word)(reg[H + ixy] << 8) + reg[L + ixy];
							break;
					}
					break;
					
				case			2:// JP cc,nn
					nn = Fetch16();
					
					if (gflag(y))
						regPC = nn;
						
					break;
					
				case			3:// assorted op
					switch (y)
					{
						case			0:// JP nn
							regPC = Fetch16();
							break;
							
						case			1:// CB prefix
							if (ixy > 0)
							{
								icb = 1;
								ofs = (signed char)Fetch();
							}
							
							Opcode = Fetch();
							
							x = (Opcode & 0xC0) >> 6;
							y = (Opcode & 0x38) >> 3;
							z = (Opcode & 0x07);
							
							switch (x)
							{
								case			0:// rot r[z]
									nn = dreg(z);
									//bt = ReadFromdreg(nn);
									
									if (dreginmem == 0)
									   bt = reg[(byte)nn];
						            else
						               bt = RMem(nn);
									   
									switch (y)
									{
										case			0:// RLC 8-bit
											p1 = (bt << 1) + (bt >> 7);											
											fC = (bt >> 7);
											break;
											
										case			1:// RRC 8-bit
											p1 = (bt >> 1) + (bt << 7);											
											fC = (bt & 1);
											break;
											
										case			2:// RL 9-bit
											p1 = (bt << 1) + fC;											
											fC = (bt >> 7);
											break;
											
										case			3:// RR 9-bit
											p1 = (bt >> 1) + (fC << 7);											
											fC =(bt & 1);
											break;
											
										case			4:// SLA
											p1 = (bt << 1);											
											fC = (bt >> 7);
											break;
											
										case			5:// SRA
											p1 = (bt >> 1) + (bt & 0x80);											
											fC = (bt & 1);
											break;
											
										case			6:// SLL
											p1 = (bt << 1) + 1;											
											fC = (bt >> 7);
											break;
											
										case			7:// SRL
											p1 = (bt >> 1);											
											fC = (bt & 1);
											break;
									}
									
									if (dreginmem == 0)
									   reg[(byte)nn] = (byte)p1;
						            else
						               WMem(nn, (byte)p1);
									   
									fS = ((p1 & 0x80) != 0);
									fZ = ((p1 & 0xFF) == 0);
									fP = (pt[p1 >> 4] + pt[p1 & 0xf]);
									fN = 0;
									fH = 0;
									break;
									
								case			1:// BIT y,r[z]
									bt = get8op(z);
									fZ = ((bt & (1 << y)) == 0);
									fP = fZ;
									fH = 1;
									fN = 0;
									
									if ((1 << y) == 128)
									   fS = (!(bt & (1 << y)) == 0);
									else
                                       fS = 0;
									break;
									
								case			2:// RES y,r[z]
									nn = dreg(z);
									
									if (dreginmem == 1)
									{
										bt = RMem(nn);
										bt &= (byte)(~(1 << y));
										WMem(nn, bt);
									}
									else
										reg[nn] &= (byte)(~(1 << y));
										
									break;
									
								case			3:// SET y,r[z]
									nn = dreg(z);
									
									if (dreginmem == 1)
									{
										bt = RMem(nn);
										bt |= (byte)(1 << y);
										WMem(nn, bt);
									}
									else
										reg[nn] |= (byte)(1 << y);
										
									break;
							}
							break;
							
						case			2:// OUT (n),A 
							Fetch();
							break;
							
						case			3:// IN A,(n)
							Fetch();
							fN = 0;
							fH = 0;
							break;
							
						case			4:// EX (SP),HL
							nn = dreg(L);
							
							bt = RMem(regSP);
							WMem(regSP, ReadFromdreg(nn));
							WriteFromdreg(nn, bt);
							
							regSP++;
							nn--;
							
							bt = RMem(regSP);
							WMem(regSP, ReadFromdreg(nn));
							WriteFromdreg(nn, bt);
							
							regSP--;
							break;
							
						case			5:// EX DE,HL
							nn = (word)(reg[D] << 8) + reg[E];
							
							set16op(DE, (word)(reg[H + ixy] << 8) + reg[L + ixy]);
							
							set16op(HL, nn);
							break;
							
						case			6:// DI
						case			7:// EI
							break;
					}
					break;
					
				case			4:// call cc,nn
					nn = Fetch16();
					
					if (gflag(y))
					{
						if (nn == CX80_SYSTEM_CALL_ADDR)
							CX80_Do_SystemCalls();
						else						
						{
							Push(regPC);
							regPC = nn;
						}
					}
					break;
					
				case			5:// push call and other
					switch (y)
					{
						case			0:// PUSH BC
							Push((word)(reg[B] << 8) + reg[C]);
							break;
							
						case			1:// CALL nn
							nn = Fetch16();
							
							if (nn == CX80_SYSTEM_CALL_ADDR)
								CX80_Do_SystemCalls();
							else
							{
								Push(regPC);
							
								regPC = nn;
							}
							break;
							
						case			2:// PUSH DE
							Push((word)(reg[D] << 8) + reg[E]);
							break;
							
						case			3:// DD IX+n prefix
							ixy = 4;
							
							CX80_Z80Run();
							
							break;
							
						case			4: // PUSH HL
							Push((word)(reg[H + ixy] << 8) + reg[L + ixy]);
							break;
							
						case			5:// ED
							Opcode = Fetch();
							
							x = (Opcode & 0xC0) >> 6;
							y = (Opcode & 0x38) >> 3;
							z = (Opcode & 0x07);
							
							switch (x)
							{
								case			0: break;	// NONI 0
								
								case			1:
									switch (z)
									{
										case			0:// IN r,(C)
											fN = 0;
											fH = 0;
											break;
											
										case			1: break;	// OUT (C),r
										
										case			2:
											if (y & 1) // ADC HL,rr
											{
												rl = (word)((reg[H + ixy] << 8));
												rl += (long)(reg[L + ixy]);
												rl += fC;
												
												//rl += (long)((reg[H + ixy] << 8)) + (long)(reg[L + ixy]);
												rl += (long)(get16op(y >> 1));
												
												reg[H + ixy] = (word)(rl >> 8);
												reg[L + ixy] = ((word)(rl)) & 0xFF;
												
												fN = 0;
												fC = ((rl > 65535) || (rl < 0));
												fP = ((rl > 32767) || (rl < -32768));
												fS = ((rl & 0x8000) != 0);
												fZ = ((rl & 0xFFFF) == 0);
											}
											else           // SBC HL,rr
											{
												rl = (word)((reg[H + ixy] << 8));
												rl += (long)(reg[L + ixy]);												
												
												//rl -= (long)((reg[H + ixy] << 8)) + (long)(reg[L + ixy]);
												rl -= (long)(get16op(y >> 1) + fC);
												
												reg[H + ixy] = (word)(rl >> 8);
												reg[L + ixy] = ((word)(rl)) & 0xFF;
												
												fN = 1;
												fC = ((rl > 65535) || (rl < 0));
												fP = ((rl > 32767) || (rl < -32768));
												fS = ((rl & 0x8000) != 0);
												fZ = ((rl & 0xFFFF) == 0);
											}
											break;
											
										case			3:
											nn = Fetch16();
											
											if (y & 1) // LD rr,(nn)
											{
												//rl = SRAM_Read(nn) + (SRAM_Read(nn + 1) << 8);
												p1 = RMem16(nn);
												
												set16op(y >> 1, p1);
											}
											else         // LD (nn),rr
											{
												p1 = get16op(y >> 1);
												
												WMem16(nn, p1);
											}
											break;
											
										case			4:// NEG
											reg[A] = r = -reg[A];
											fS = ((r & 0x80) != 0);
											fZ = ((r & 0xFF) == 0);
											fP = ((r > 127) || (r < -128));
											fC = ((r > 255) || (r < 0));
											fN = 1;
											break;
											
										case			5://RETI/N
											regPC = Pop();
											break;
											
										case			6: break;		// IM y
										
										case			7:
											switch (y)
											{
												case			0:// LD I,A
												case			1:// LD R,A
												case			2:// LD A,I
												case			3:// LD A,R
													break;
													
												case			4:// RRD
													nn = (word)(reg[H + ixy] << 8) + reg[L + ixy];
													bt = RMem(nn);
													bt2 = reg[A];
													reg[A] = r = (bt2 & 0xF0) + (bt & 0xF);
													
													WMem(nn, ((bt >> 4) & 0xF) + ((bt2 & 0xF) << 4));
													
													fN = 0;
													fH = 0;
													fS = ((r & 0x80) != 0);
													fZ = ((r & 0xFF) == 0);
													fP = (pt[r >> 4] + pt[r & 0xf]);
													break;
													
												case			5:// RLD
													nn = (word)(reg[H + ixy] << 8) + reg[L + ixy];
													bt = RMem(nn);
													bt2 = reg[A];
													reg[A] = r = (bt2 & 0xF0) + (bt >> 4);
													
													WMem(nn, ((bt << 4) & 0xF0) + (bt2 & 0xF));
													
													fN = 0;
													fH = 0;
													fS = ((r & 0x80) != 0);
													fZ = ((r & 0xFF) == 0);
													fP = (pt[r >> 4] + pt[r & 0xf]);
													break;
													
												case			6: //NOP
												case			7: //NOP
													break;
											}
											break;
									}
									break;
									
								case			2://LDIR/CP/IN/OUT...
									if ((z <= 3) && (y >= 4))
									{
										word ss = (word)(reg[H + ixy] << 8) + reg[L + ixy];
										word dd = (word)(reg[D] << 8) + reg[E];
										word cc = (word)(reg[B] << 8) + reg[C];
										
										bt = 1;
										
										do 
										{
											switch (z)
											{
												case			0:// LDxx
													WMem(dd, RMem(ss));
													break;
													
												case			1:// CPxx
													bt = (RMem(ss) - reg[A]);
													break;
													
												case			2:
												case			3:
													break;
											}
											
											bt2 = y - 4;
											
											if (bt2 & 1)	//dec
											{
												ss--;
												dd--;
											}
											else
											{
												ss++;
												dd++;
											}
											
											cc--;
											
										} while (bt && (bt2 & 2) && (cc > 0));
										
										set16op(DE, dd);
										set16op(HL, ss);
										set16op(BC, cc);
									}
									break;
									
								case			3:
									break;
							}
							break;
							
						case				6:// PUSH AF
							nn=0;
							nn += (fS) ? 1 : 0;  nn<<=1;
							nn += (fZ) ? 1 : 0;  nn<<=1;
							nn += (fU5)? 1 : 0;  nn<<=1;
							nn += (fH) ? 1 : 0;  nn<<=1;
							nn += (fU3)? 1 : 0;  nn<<=1;
							nn += (fP) ? 1 : 0;  nn<<=1;
							nn += (fN) ? 1 : 0;  nn<<=1;
							nn += (fC) ? 1 : 0;  
							
							Push((word)(reg[A] << 8) + nn);
							break;
							
						case				7:// FD IY+n prefix
							ixy = 6;
							
							CX80_Z80Run();
							break;
					}
					break;
					
				case			6:// alu + immediate
					Alu(y, Fetch());
					break;
					
				case			7:// RST nn
					Push(regPC);
					
					regPC = y * 8;
					break;
			}
			break;
	}
	
	ixy = 0;
}