/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2013 Mike Chambers

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* 
  The author of the port for Arduino Corax (Rabetsky Igor)
  Copyright (C) 2017 corax89
  
  Some of the code are edited by @raspiduino
  Copyright (C) 2021 @raspiduino
*/

#include <stdint.h>
#include <stdio.h>
#include "cpu.h"

//#define DEBUG
#define CPU_8086
#define STACK_LENGTH 32

#define regax 0
#define regcx 1
#define regdx 2
#define regbx 3
#define regsp 4
#define regbp 5
#define regsi 6
#define regdi 7
#define reges 0
#define regcs 1
#define regss 2
#define regds 3

#define regal 0
#define regah 1
#define regcl 2
#define regch 3
#define regdl 4
#define regdh 5
#define regbl 6
#define regbh 7

#define StepIP(x)  ip += x
#define getmem8(x, y) read86(segbase(x) + y)
#define getmem16(x, y)  readw86(segbase(x) + y)
#define putmem8(x, y, z)  write86(segbase(x) + y, z)
#define putmem16(x, y, z) writew86(segbase(x) + y, z)
#define signext(value)  (int16_t)(int8_t)(value)
#define signext32(value)  (int32_t)(int16_t)(value)
#define getreg16(regid) regs.wordregs[regid]
#define getreg8(regid)  regs.byteregs[byteregtable[regid]]
#define putreg16(regid, writeval) regs.wordregs[regid] = writeval
#define putreg8(regid, writeval)  regs.byteregs[byteregtable[regid]] = writeval
#define getsegreg(regid)  segregs[regid]
#define putsegreg(regid, writeval)  segregs[regid] = writeval
#define segbase(x)  ((uint32_t) x << 4)

#define makeflagsword() \
  ( \
  2 | (word) cf | ((word) pf << 2) | ((word) af << 4) | ((word) zf << 6) | ((word) sf << 7) | \
  ((word) tf << 8) | ((word) ifl << 9) | ((word) df << 10) | ((word) of << 11) \
  )

union _bytewordregs_ {
  word wordregs[8];
  byte byteregs[8];
};

byte byteregtable[8] = { regal, regcl, regdl, regbl, regah, regch, regdh, regbh };

static const byte parity[0x100] = {
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

byte STACK[STACK_LENGTH];

byte opcode, segoverride, reptype, bootdrive = 0, hdcount = 0, hltstate = 0;
word segregs[4], savecs, saveip, ip, useseg, oldsp;
byte tempcf, oldcf, cf, pf, af, zf, sf, tf, ifl, df, of, mode, reg, rm;
word oper1, oper2, res16, disp16, temp16, dummy, stacksize, frametemp;
byte oper1b, oper2b, res8, disp8, temp8, nestlev, addrbyte;
uint32_t temp1, temp2, temp3, temp4, temp5, temp32, tempaddr32, ea;
int32_t result;
uint64_t totalexec;

union _bytewordregs_ regs;

byte running = 0, didbootstrap = 0;

byte vidmode;

// Interupts

void videoBIOSinterupt(){
  switch(regs.byteregs[regah]){
  case 0x9:
  /*09H писать символ/атрибут в текущей позиции курсора
     вход:  BH = номер видео страницы
     AL = записываемый символ
     CX = счетчик (сколько экземпляров символа записать)
     BL = видео атрибут (текст) или цвет (графика)
      (графические режимы: +80H означает XOR с символом на экране)*/
  case 0xA:
    /*0aH писать символ в текущей позиции курсора
      вход:  BH = номер видео страницы
      AL = записываемый символ
      CX = счетчик (сколько экземпляров символа записать)*/
    for(uint16_t j=0;j<regs.wordregs[regcx];j++)
      Serial.print((char)(regs.byteregs[regal]));
    break;
  case 0xE:
  /*0eH писать символ на активную видео страницу (эмуляция телетайпа)
    вход:  AL = записываемый символ (использует существующий атрибут)
    BL = цвет переднего плана (для графических режимов)*/
    Serial.print((char)(regs.byteregs[regal]));
    break;
#ifdef DEBUG
    default:
      Serial.print("undefined videoBIOS interupt ");
      Serial.print(regs.byteregs[regah],HEX);
#endif
    }
}

void keyBIOSinterupt(){
  switch(regs.byteregs[regah]){
    case 0x0:
    /*00H читать (ожидать) следующую нажатую клавишу
    выход: AL = ASCII символ (если AL=0, AH содержит расширенный код ASCII )
          AH = сканкод  или расширенный код ASCII*/
    while(Serial.available() == 0){
      
    }
    regs.byteregs[regal] = (uint8_t) Serial.read();
    break;
#ifdef DEBUG
    default:
      Serial.print("undefined keyBIOS interupt ");
      Serial.print(regs.byteregs[regah],HEX);
#endif
  }
}

void DOSinterupt(){
  uint16_t adrs;
  switch(regs.byteregs[regah]){
    case 0x01:
      /*Вход AH = 01H
      Выход AL = символ, полученный из стандартного ввода
      Считывает (ожидает) символ со стандартного входного устройства. Отображает этот символ на стандартное выходное устройство (эхо)*/
      while(Serial.available() == 0){
        
      }
      regs.byteregs[regal] = (uint8_t) Serial.read();
      Serial.print((char)regs.byteregs[regal]);
      break; 
    case 0x02:
      Serial.print((char)regs.byteregs[regdl]);
      break;
    case 0x09:
      // AH=09h - вывод строки из DS:DX.
      for(uint8_t i = 0; i < 255; i++){
        char ch = (char)read86((segregs[regds] << 4) + regs.wordregs[regdx] + i);
        if(ch != '$')
          Serial.print(ch);
        else{
          regs.byteregs[regal] = 0x24;
          return;
        }
      }
      break;
    case 0x0a:
      /*DOS Fn 0aH: ввод строки в буфеp
      Вход AH = 0aH
      DS:DX = адрес входного буфера (смотри ниже)
      Выход нет = буфер содержит ввод, заканчивающийся символом CR (ASCII 0dH)*/
      adrs = (segregs[regds] << 4) + regs.wordregs[regdx];
      uint8_t length=0;
      char ch;
      ch=Serial.read();
      while(true){
        while(Serial.available() == 0){
          
        }
        ch=Serial.read();
        Serial.print(ch);
        if(ch=='\n' || ch=='\r')
          break;
        write86(adrs+length+2,(uint8_t) ch);
        length++;
        if(length>read86(adrs) || length>255)
          break;
      }
      write86(adrs+1,length);//записываем действительную длину данных
      write86(adrs+length+3,'$');
      break;
#ifdef DEBUG
    default:
      Serial.print("undefined DOS interupt ");
      Serial.print(regs.byteregs[regah],HEX);
#endif
    }
}

void decodeflagsword( word x) { 
  temp16 = x; 
  cf = temp16 & 1; 
  pf = (temp16 >> 2) & 1; 
  af = (temp16 >> 4) & 1; 
  zf = (temp16 >> 6) & 1; 
  sf = (temp16 >> 7) & 1; 
  tf = (temp16 >> 8) & 1; 
  ifl = (temp16 >> 9) & 1; 
  df = (temp16 >> 10) & 1; 
  of = (temp16 >> 11) & 1; 
}


void modregrm() { 
  addrbyte = getmem8(segregs[regcs], ip);
  StepIP(1);
  mode = addrbyte >> 6; 
  reg = (addrbyte >> 3) & 7; 
  rm = addrbyte & 7; 
  switch(mode){ 
    case 0: 
      if(rm == 6) { 
        disp16 = getmem16(segregs[regcs], ip); 
        StepIP(2); 
      } 
      if(((rm == 2) || (rm == 3)) && !segoverride) { 
        useseg = segregs[regss]; 
      } 
      break; 
    case 1: 
      disp16 = signext(getmem8(segregs[regcs], ip)); 
      StepIP(1); 
      if(((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) { 
        useseg = segregs[regss]; 
      } 
      break; 
    case 2: 
      disp16 = getmem16(segregs[regcs], ip); 
      StepIP(2); 
      if(((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) { 
        useseg = segregs[regss]; 
      } 
      break; 
    default: 
      disp8 = 0; 
      disp16 = 0; 
  } 
}

void write86 (uint32_t addr32, byte value) {
  if(addr32 > 0xFFFF - STACK_LENGTH){
    STACK[tempaddr32] = value;
  }
  else if(addr32 >= 0x7100 && addr32 < RAM_LENGTH + 0x7100){
    tempaddr32 = (addr32 & 0xFFFFF) - 0x7100;
    //RAM[tempaddr32] = value;
    ramwrite(tempaddr32, value); // Write to virtual RAM
  }
}

void writew86 (uint32_t addr32, word value) {
  write86 (addr32, (byte) value);
  write86 (addr32 + 1, (byte) (value >> 8) );
}

byte read86 (uint32_t addr32) {
  if(addr32 > 0xFFFF - STACK_LENGTH){
    return STACK[tempaddr32];
  }
  else if(addr32 >= 0x7100 && addr32 < RAM_LENGTH + 0x7100){
    addr32 &= 0xFFFFF;
    addr32 -= 0x7100; 
    return (ramread(addr32));
  }
  return 0x90;
}

word readw86 (uint32_t addr32) {
  return ( (word) read86 (addr32) | (word) (read86 (addr32 + 1) << 8) );
}

void flag_szp8 (byte value) {
  if (!value) {
      zf = 1;
    }
  else {
      zf = 0; /* set or clear zero flag */
    }

  if (value & 0x80) {
      sf = 1;
    }
  else {
      sf = 0; /* set or clear sign flag */
    }

  pf = parity[value]; /* retrieve parity state from lookup table */
}

void flag_szp16 (word value) {
  if (!value) {
      zf = 1;
    }
  else {
      zf = 0; /* set or clear zero flag */
    }

  if (value & 0x8000) {
      sf = 1;
    }
  else {
      sf = 0; /* set or clear sign flag */
    }

  pf = parity[value & 255]; /* retrieve parity state from lookup table */
}

void flag_log8 (byte value) {
  flag_szp8 (value);
  cf = 0;
  of = 0; /* bitwise logic ops always clear carry and overflow */
}

void flag_log16 (word value) {
  flag_szp16 (value);
  cf = 0;
  of = 0; /* bitwise logic ops always clear carry and overflow */
}

void flag_adc8 (byte v1, byte v2, byte v3) {

  /* v1 = destination operand, v2 = source operand, v3 = carry flag */
  word  dst;

  dst = (word) v1 + (word) v2 + (word) v3;
  flag_szp8 ( (byte) dst);
  if ( ( (dst ^ v1) & (dst ^ v2) & 0x80) == 0x80) {
      of = 1;
    }
  else {
      of = 0; /* set or clear overflow flag */
    }

  if (dst & 0xFF00) {
      cf = 1;
    }
  else {
      cf = 0; /* set or clear carry flag */
    }

  if ( ( (v1 ^ v2 ^ dst) & 0x10) == 0x10) {
      af = 1;
    }
  else {
      af = 0; /* set or clear auxilliary flag */
    }
}

void flag_adc16 (word v1, word v2, word v3) {

  uint32_t  dst;

  dst = (uint32_t) v1 + (uint32_t) v2 + (uint32_t) v3;
  flag_szp16 ( (word) dst);
  if ( ( ( (dst ^ v1) & (dst ^ v2) ) & 0x8000) == 0x8000) {
      of = 1;
    }
  else {
      of = 0;
    }

  if (dst & 0xFFFF0000) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( ( (v1 ^ v2 ^ dst) & 0x10) == 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_add8 (byte v1, byte v2) {
  /* v1 = destination operand, v2 = source operand */
  word  dst;

  dst = (word) v1 + (word) v2;
  flag_szp8 ( (byte) dst);
  if (dst & 0xFF00) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( ( (dst ^ v1) & (dst ^ v2) & 0x80) == 0x80) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( ( (v1 ^ v2 ^ dst) & 0x10) == 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_add16 (word v1, word v2) {
  /* v1 = destination operand, v2 = source operand */
  uint32_t  dst;

  dst = (uint32_t) v1 + (uint32_t) v2;
  flag_szp16 ( (word) dst);
  if (dst & 0xFFFF0000) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( ( (dst ^ v1) & (dst ^ v2) & 0x8000) == 0x8000) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( ( (v1 ^ v2 ^ dst) & 0x10) == 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_sbb8 (byte v1, byte v2, byte v3) {

  /* v1 = destination operand, v2 = source operand, v3 = carry flag */
  word  dst;

  v2 += v3;
  dst = (word) v1 - (word) v2;
  flag_szp8 ( (byte) dst);
  if (dst & 0xFF00) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( (dst ^ v1) & (v1 ^ v2) & 0x80) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( (v1 ^ v2 ^ dst) & 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_sbb16 (word v1, word v2, word v3) {

  /* v1 = destination operand, v2 = source operand, v3 = carry flag */
  uint32_t  dst;

  v2 += v3;
  dst = (uint32_t) v1 - (uint32_t) v2;
  flag_szp16 ( (word) dst);
  if (dst & 0xFFFF0000) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( (dst ^ v1) & (v1 ^ v2) & 0x8000) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( (v1 ^ v2 ^ dst) & 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_sub8 (byte v1, byte v2) {

  /* v1 = destination operand, v2 = source operand */
  word  dst;

  dst = (word) v1 - (word) v2;
  flag_szp8 ( (byte) dst);
  if (dst & 0xFF00) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( (dst ^ v1) & (v1 ^ v2) & 0x80) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( (v1 ^ v2 ^ dst) & 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void flag_sub16 (word v1, word v2) {

  /* v1 = destination operand, v2 = source operand */
  uint32_t  dst;

  dst = (uint32_t) v1 - (uint32_t) v2;
  flag_szp16 ( (word) dst);
  if (dst & 0xFFFF0000) {
      cf = 1;
    }
  else {
      cf = 0;
    }

  if ( (dst ^ v1) & (v1 ^ v2) & 0x8000) {
      of = 1;
    }
  else {
      of = 0;
    }

  if ( (v1 ^ v2 ^ dst) & 0x10) {
      af = 1;
    }
  else {
      af = 0;
    }
}

void op_adc8() {
  res8 = oper1b + oper2b + cf;
  flag_adc8 (oper1b, oper2b, cf);
}

void op_adc16() {
  res16 = oper1 + oper2 + cf;
  flag_adc16 (oper1, oper2, cf);
}

void op_add8() {
  res8 = oper1b + oper2b;
  flag_add8 (oper1b, oper2b);
}

void op_add16() {
  res16 = oper1 + oper2;
  flag_add16 (oper1, oper2);
}

void op_and8() {
  res8 = oper1b & oper2b;
  flag_log8 (res8);
}

void op_and16() {
  res16 = oper1 & oper2;
  flag_log16 (res16);
}

void op_or8() {
  res8 = oper1b | oper2b;
  flag_log8 (res8);
}

void op_or16() {
  res16 = oper1 | oper2;
  flag_log16 (res16);
}

void op_xor8() {
  res8 = oper1b ^ oper2b;
  flag_log8 (res8);
}

void op_xor16() {
  res16 = oper1 ^ oper2;
  flag_log16 (res16);
}

void op_sub8() {
  res8 = oper1b - oper2b;
  flag_sub8 (oper1b, oper2b);
}

void op_sub16() {
  res16 = oper1 - oper2;
  flag_sub16 (oper1, oper2);
}

void op_sbb8() {
  res8 = oper1b - (oper2b + cf);
  flag_sbb8 (oper1b, oper2b, cf);
}

void op_sbb16() {
  res16 = oper1 - (oper2 + cf);
  flag_sbb16 (oper1, oper2, cf);
}

void getea (byte rmval) {
  uint32_t  tempea;

  tempea = 0;
  switch (mode) {
      case 0:
        switch (rmval) {
            case 0:
              tempea = regs.wordregs[regbx] + regs.wordregs[regsi];
              break;
            case 1:
              tempea = regs.wordregs[regbx] + regs.wordregs[regdi];
              break;
            case 2:
              tempea = regs.wordregs[regbp] + regs.wordregs[regsi];
              break;
            case 3:
              tempea = regs.wordregs[regbp] + regs.wordregs[regdi];
              break;
            case 4:
              tempea = regs.wordregs[regsi];
              break;
            case 5:
              tempea = regs.wordregs[regdi];
              break;
            case 6:
              tempea = disp16;
              break;
            case 7:
              tempea = regs.wordregs[regbx];
              break;
          }
        break;

      case 1:
      case 2:
        switch (rmval) {
            case 0:
              tempea = regs.wordregs[regbx] + regs.wordregs[regsi] + disp16;
              break;
            case 1:
              tempea = regs.wordregs[regbx] + regs.wordregs[regdi] + disp16;
              break;
            case 2:
              tempea = regs.wordregs[regbp] + regs.wordregs[regsi] + disp16;
              break;
            case 3:
              tempea = regs.wordregs[regbp] + regs.wordregs[regdi] + disp16;
              break;
            case 4:
              tempea = regs.wordregs[regsi] + disp16;
              break;
            case 5:
              tempea = regs.wordregs[regdi] + disp16;
              break;
            case 6:
              tempea = regs.wordregs[regbp] + disp16;
              break;
            case 7:
              tempea = regs.wordregs[regbx] + disp16;
              break;
          }
        break;
    }

  ea = (tempea & 0xFFFF) + (useseg << 4);
}

void push (word pushval) {
  regs.wordregs[regsp] = regs.wordregs[regsp] - 2;
  putmem16 (segregs[regss], regs.wordregs[regsp], pushval);
}

word pop() {

  word  tempval;

  tempval = getmem16 (segregs[regss], regs.wordregs[regsp]);
  regs.wordregs[regsp] = regs.wordregs[regsp] + 2;
  return tempval;
}

void reset86() {
  segregs[regcs] = 0xFFFF;
  ip = 0x0100;
  hltstate = 0;
}

word readrm16 (byte rmval) {
  if (mode < 3) {
      getea (rmval);
      return read86 (ea) | ( (word) read86 (ea + 1) << 8);
    }
  else {
      return getreg16 (rmval);
    }
}

byte readrm8 (byte rmval) {
  if (mode < 3) {
      getea (rmval);
      return read86 (ea);
    }
  else {
      return getreg8 (rmval);
    }
}

void writerm16 (byte rmval, word value) {
  if (mode < 3) {
      getea (rmval);
      write86 (ea, value & 0xFF);
      write86 (ea + 1, value >> 8);
    }
  else {
      putreg16 (rmval, value);
    }
}

void writerm8 (byte rmval, byte value) {
  if (mode < 3) {
      getea (rmval);
      write86 (ea, value);
    }
  else {
      putreg8 (rmval, value);
    }
}


void intcall86 (byte intnum) {
  static word lastint10ax;
  word oldregax;

  if (intnum == 0x19) didbootstrap = 1;

  switch (intnum) {
      case 0x10:
        videoBIOSinterupt();
        break;
      case 0x16:
        keyBIOSinterupt();
        break;
      case 0x21:
        DOSinterupt();
        break;
      default:
        push (makeflagsword() );
        push (segregs[regcs]);
        push (ip);
        segregs[regcs] = getmem16 (0, (word) intnum * 4 + 2);
        ip = getmem16 (0, (word) intnum * 4);
        ifl = 0;
        tf = 0;
    }
}


byte op_grp2_8 (byte cnt) {

  word  s;
  word  shift;
  word  oldcf;
  word  msb;

  s = oper1b;
  oldcf = cf;
  switch (reg) {
      case 0: /* ROL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            if (s & 0x80) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = s << 1;
            s = s | cf;
          }

        if (cnt == 1) {
            //of = cf ^ ( (s >> 7) & 1);
          if ((s & 0x80) && cf) of = 1; else of = 0;
          } else of = 0;
        break;

      case 1: /* ROR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            cf = s & 1;
            s = (s >> 1) | (cf << 7);
          }

        if (cnt == 1) {
            of = (s >> 7) ^ ( (s >> 6) & 1);
          }
        break;

      case 2: /* RCL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            oldcf = cf;
            if (s & 0x80) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = s << 1;
            s = s | oldcf;
          }

        if (cnt == 1) {
            of = cf ^ ( (s >> 7) & 1);
          }
        break;

      case 3: /* RCR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            oldcf = cf;
            cf = s & 1;
            s = (s >> 1) | (oldcf << 7);
          }

        if (cnt == 1) {
            of = (s >> 7) ^ ( (s >> 6) & 1);
          }
        break;

      case 4:
      case 6: /* SHL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            if (s & 0x80) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = (s << 1) & 0xFF;
          }

        if ( (cnt == 1) && (cf == (s >> 7) ) ) {
            of = 0;
          }
        else {
            of = 1;
          }

        flag_szp8 ( (byte) s);
        break;

      case 5: /* SHR r/m8 */
        if ( (cnt == 1) && (s & 0x80) ) {
            of = 1;
          }
        else {
            of = 0;
          }

        for (shift = 1; shift <= cnt; shift++) {
            cf = s & 1;
            s = s >> 1;
          }

        flag_szp8 ( (byte) s);
        break;

      case 7: /* SAR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            msb = s & 0x80;
            cf = s & 1;
            s = (s >> 1) | msb;
          }

        of = 0;
        flag_szp8 ( (byte) s);
        break;
    }

  return s & 0xFF;
}

word op_grp2_16 (byte cnt) {

  uint32_t  s;
  uint32_t  shift;
  uint32_t  oldcf;
  uint32_t  msb;

  s = oper1;
  oldcf = cf;
  switch (reg) {
      case 0: /* ROL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            if (s & 0x8000) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = s << 1;
            s = s | cf;
          }

        if (cnt == 1) {
            of = cf ^ ( (s >> 15) & 1);
          }
        break;

      case 1: /* ROR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            cf = s & 1;
            s = (s >> 1) | (cf << 15);
          }

        if (cnt == 1) {
            of = (s >> 15) ^ ( (s >> 14) & 1);
          }
        break;

      case 2: /* RCL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            oldcf = cf;
            if (s & 0x8000) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = s << 1;
            s = s | oldcf;
          }

        if (cnt == 1) {
            of = cf ^ ( (s >> 15) & 1);
          }
        break;

      case 3: /* RCR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            oldcf = cf;
            cf = s & 1;
            s = (s >> 1) | (oldcf << 15);
          }

        if (cnt == 1) {
            of = (s >> 15) ^ ( (s >> 14) & 1);
          }
        break;

      case 4:
      case 6: /* SHL r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            if (s & 0x8000) {
                cf = 1;
              }
            else {
                cf = 0;
              }

            s = (s << 1) & 0xFFFF;
          }

        if ( (cnt == 1) && (cf == (s >> 15) ) ) {
            of = 0;
          }
        else {
            of = 1;
          }

        flag_szp16 ( (word) s);
        break;

      case 5: /* SHR r/m8 */
        if ( (cnt == 1) && (s & 0x8000) ) {
            of = 1;
          }
        else {
            of = 0;
          }

        for (shift = 1; shift <= cnt; shift++) {
            cf = s & 1;
            s = s >> 1;
          }

        flag_szp16 ( (word) s);
        break;

      case 7: /* SAR r/m8 */
        for (shift = 1; shift <= cnt; shift++) {
            msb = s & 0x8000;
            cf = s & 1;
            s = (s >> 1) | msb;
          }

        of = 0;
        flag_szp16 ( (word) s);
        break;
    }

  return (word) s & 0xFFFF;
}

void op_div8 (word valdiv, byte divisor) {
  if (divisor == 0) {
      intcall86 (0);
      return;
    }

  if ( (valdiv / (word) divisor) > 0xFF) {
      intcall86 (0);
      return;
    }

  regs.byteregs[regah] = valdiv % (word) divisor;
  regs.byteregs[regal] = valdiv / (word) divisor;
}

void op_idiv8 (word valdiv, byte divisor) {

  word  s1;
  word  s2;
  word  d1;
  word  d2;
  int sign;

  if (divisor == 0) {
      intcall86 (0);
      return;
    }

  s1 = valdiv;
  s2 = divisor;
  sign = ( ( (s1 ^ s2) & 0x8000) != 0);
  s1 = (s1 < 0x8000) ? s1 : ( (~s1 + 1) & 0xffff);
  s2 = (s2 < 0x8000) ? s2 : ( (~s2 + 1) & 0xffff);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFF00) {
      intcall86 (0);
      return;
    }

  if (sign) {
      d1 = (~d1 + 1) & 0xff;
      d2 = (~d2 + 1) & 0xff;
    }

  regs.byteregs[regah] = (byte) d2;
  regs.byteregs[regal] = (byte) d1;
}

void op_grp3_8() {
  oper1 = signext (oper1b);
  oper2 = signext (oper2b);
  switch (reg) {
      case 0:
      case 1: /* TEST */
        flag_log8 (oper1b & getmem8 (segregs[regcs], ip) );
        StepIP (1);
        break;

      case 2: /* NOT */
        res8 = ~oper1b;
        break;

      case 3: /* NEG */
        res8 = (~oper1b) + 1;
        flag_sub8 (0, oper1b);
        if (res8 == 0) {
            cf = 0;
          }
        else {
            cf = 1;
          }
        break;

      case 4: /* MUL */
        temp1 = (uint32_t) oper1b * (uint32_t) regs.byteregs[regal];
        regs.wordregs[regax] = temp1 & 0xFFFF;
        flag_szp8 ( (byte) temp1);
        if (regs.byteregs[regah]) {
            cf = 1;
            of = 1;
          }
        else {
            cf = 0;
            of = 0;
          }
        break;

      case 5: /* IMUL */
        oper1 = signext (oper1b);
        temp1 = signext (regs.byteregs[regal]);
        temp2 = oper1;
        if ( (temp1 & 0x80) == 0x80) {
            temp1 = temp1 | 0xFFFFFF00;
          }

        if ( (temp2 & 0x80) == 0x80) {
            temp2 = temp2 | 0xFFFFFF00;
          }

        temp3 = (temp1 * temp2) & 0xFFFF;
        regs.wordregs[regax] = temp3 & 0xFFFF;
        if (regs.byteregs[regah]) {
            cf = 1;
            of = 1;
          }
        else {
            cf = 0;
            of = 0;
          }
        break;

      case 6: /* DIV */
        op_div8 (regs.wordregs[regax], oper1b);
        break;

      case 7: /* IDIV */
        op_idiv8 (regs.wordregs[regax], oper1b);
        break;
    }
}

void op_div16 (uint32_t valdiv, word divisor) {
  if (divisor == 0) {
      intcall86 (0);
      return;
    }

  if ( (valdiv / (uint32_t) divisor) > 0xFFFF) {
      intcall86 (0);
      return;
    }

  regs.wordregs[regdx] = valdiv % (uint32_t) divisor;
  regs.wordregs[regax] = valdiv / (uint32_t) divisor;
}

void op_idiv16 (uint32_t valdiv, word divisor) {

  uint32_t  d1;
  uint32_t  d2;
  uint32_t  s1;
  uint32_t  s2;
  int sign;

  if (divisor == 0) {
      intcall86 (0);
      return;
    }

  s1 = valdiv;
  s2 = divisor;
  s2 = (s2 & 0x8000) ? (s2 | 0xffff0000) : s2;
  sign = ( ( (s1 ^ s2) & 0x80000000) != 0);
  s1 = (s1 < 0x80000000) ? s1 : ( (~s1 + 1) & 0xffffffff);
  s2 = (s2 < 0x80000000) ? s2 : ( (~s2 + 1) & 0xffffffff);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFFFF0000) {
      intcall86 (0);
      return;
    }

  if (sign) {
      d1 = (~d1 + 1) & 0xffff;
      d2 = (~d2 + 1) & 0xffff;
    }

  regs.wordregs[regax] = d1;
  regs.wordregs[regdx] = d2;
}

void op_grp3_16() {
  switch (reg) {
      case 0:
      case 1: /* TEST */
        flag_log16 (oper1 & getmem16 (segregs[regcs], ip) );
        StepIP (2);
        break;

      case 2: /* NOT */
        res16 = ~oper1;
        break;

      case 3: /* NEG */
        res16 = (~oper1) + 1;
        flag_sub16 (0, oper1);
        if (res16) {
            cf = 1;
          }
        else {
            cf = 0;
          }
        break;

      case 4: /* MUL */
        temp1 = (uint32_t) oper1 * (uint32_t) regs.wordregs[regax];
        regs.wordregs[regax] = temp1 & 0xFFFF;
        regs.wordregs[regdx] = temp1 >> 16;
        flag_szp16 ( (word) temp1);
        if (regs.wordregs[regdx]) {
            cf = 1;
            of = 1;
          }
        else {
            cf = 0;
            of = 0;
          }
        break;

      case 5: /* IMUL */
        temp1 = regs.wordregs[regax];
        temp2 = oper1;
        if (temp1 & 0x8000) {
            temp1 |= 0xFFFF0000;
          }

        if (temp2 & 0x8000) {
            temp2 |= 0xFFFF0000;
          }

        temp3 = temp1 * temp2;
        regs.wordregs[regax] = temp3 & 0xFFFF;  /* into register ax */
        regs.wordregs[regdx] = temp3 >> 16; /* into register dx */
        if (regs.wordregs[regdx]) {
            cf = 1;
            of = 1;
          }
        else {
            cf = 0;
            of = 0;
          }
        break;

      case 6: /* DIV */
        op_div16 ( ( (uint32_t) regs.wordregs[regdx] << 16) + regs.wordregs[regax], oper1);
        break;

      case 7: /* DIV */
        op_idiv16 ( ( (uint32_t) regs.wordregs[regdx] << 16) + regs.wordregs[regax], oper1);
        break;
    }
}

void op_grp5() {
  switch (reg) {
      case 0: /* INC Ev */
        oper2 = 1;
        tempcf = cf;
        op_add16();
        cf = tempcf;
        writerm16 (rm, res16);
        break;

      case 1: /* DEC Ev */
        oper2 = 1;
        tempcf = cf;
        op_sub16();
        cf = tempcf;
        writerm16 (rm, res16);
        break;

      case 2: /* CALL Ev */
        push (ip);
        ip = oper1;
        break;

      case 3: /* CALL Mp */
        push (segregs[regcs]);
        push (ip);
        getea (rm);
        ip = (word) read86 (ea) + (word) read86 (ea + 1) * 256;
        segregs[regcs] = (word) read86 (ea + 2) + (word) read86 (ea + 3) * 256;
        break;

      case 4: /* JMP Ev */
        ip = oper1;
        break;

      case 5: /* JMP Mp */
        getea (rm);
        ip = (word) read86 (ea) + (word) read86 (ea + 1) * 256;
        segregs[regcs] = (word) read86 (ea + 2) + (word) read86 (ea + 3) * 256;
        break;

      case 6: /* PUSH Ev */
        push (oper1);
        break;
    }
}

void init86 (){
  regs.wordregs[regax] = 0;
  regs.wordregs[regbx] = 0;
  regs.wordregs[regcx] = 0x001E;
  regs.wordregs[regdx] = 0;
  segregs[regcs]= 0x0700 ;
  ip = 0x100;
  segregs[regss] = 0x0700;
  regs.wordregs[regsp] = 0xFFFE;
  regs.wordregs[regbp] = 0;
  regs.wordregs[regsi] = 0;
  regs.wordregs[regdi] = 0;
  segregs[regds] = 0x0700;
  segregs[reges] = 0x0700;
}

void exec86 (uint32_t execloops) {

  uint32_t loopcount;
  byte docontinue;
  static word firstip;
  static word trap_toggle = 0;

  //counterticks = (uint64_t) ( (double) timerfreq / (double) 65536.0);

  for (loopcount = 0; loopcount < execloops; loopcount++) {

      if (tf) {
          trap_toggle = 1;
        }
      else {
          trap_toggle = 0;
        }

      if (hltstate) goto skipexecution;

      reptype = 0;
      segoverride = 0;
      useseg = segregs[regds];
      docontinue = 0;
      firstip = ip;

      if ( (segregs[regcs] == 0xF000) && (ip == 0xE066) ) didbootstrap = 0; //detect if we hit the BIOS entry point to clear didbootstrap because we've rebooted

      while (!docontinue) {
          segregs[regcs] = segregs[regcs] & 0xFFFF;
          ip = ip & 0xFFFF;
          savecs = segregs[regcs];
          saveip = ip;
          opcode = getmem8 (segregs[regcs], ip);
          StepIP (1);

#ifdef DEBUG
          Serial.print("op:");
          Serial.print(opcode, HEX);
          Serial.print(" ip:");
          Serial.print(ip, HEX);
          Serial.print(" ax:");
          Serial.print(regs.wordregs[regax], HEX);
          Serial.print(" bx:");
          Serial.print(regs.wordregs[regbx], HEX);
          Serial.print(" cx:");
          Serial.print(regs.wordregs[regcx],  HEX);
          Serial.print(" dx:");
          Serial.println(regs.wordregs[regdx], HEX);
#endif

          switch (opcode) {
                /* segment prefix check */
              case 0x2E:  /* segment segregs[regcs] */
                useseg = segregs[regcs];
                segoverride = 1;
                break;

              case 0x3E:  /* segment segregs[regds] */
                useseg = segregs[regds];
                segoverride = 1;
                break;

              case 0x26:  /* segment segregs[reges] */
                useseg = segregs[reges];
                segoverride = 1;
                break;

              case 0x36:  /* segment segregs[regss] */
                useseg = segregs[regss];
                segoverride = 1;
                break;

                /* repetition prefix check */
              case 0xF3:  /* REP/REPE/REPZ */
                reptype = 1;
                break;

              case 0xF2:  /* REPNE/REPNZ */
                reptype = 2;
                break;

              default:
                docontinue = 1;
                break;
            }
        }

      totalexec++;

      switch (opcode) {
          case 0x0: /* 00 ADD Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_add8();
            writerm8 (rm, res8);
            break;

          case 0x1: /* 01 ADD Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_add16();
            writerm16 (rm, res16);
            break;

          case 0x2: /* 02 ADD Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_add8();
            putreg8 (reg, res8);
            break;

          case 0x3: /* 03 ADD Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_add16();
            putreg16 (reg, res16);
            break;

          case 0x4: /* 04 ADD regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_add8();
            regs.byteregs[regal] = res8;
            break;

          case 0x5: /* 05 ADD eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_add16();
            regs.wordregs[regax] = res16;
            break;

          case 0x6: /* 06 PUSH segregs[reges] */
            push (segregs[reges]);
            break;

          case 0x7: /* 07 POP segregs[reges] */
            segregs[reges] = pop();
            break;

          case 0x8: /* 08 OR Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_or8();
            writerm8 (rm, res8);
            break;

          case 0x9: /* 09 OR Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_or16();
            writerm16 (rm, res16);
            break;

          case 0xA: /* 0A OR Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_or8();
            putreg8 (reg, res8);
            break;

          case 0xB: /* 0B OR Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_or16();
            if ( (oper1 == 0xF802) && (oper2 == 0xF802) ) {
                sf = 0; /* cheap hack to make Wolf 3D think we're a 286 so it plays */
              }

            putreg16 (reg, res16);
            break;

          case 0xC: /* 0C OR regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_or8();
            regs.byteregs[regal] = res8;
            break;

          case 0xD: /* 0D OR eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_or16();
            regs.wordregs[regax] = res16;
            break;

          case 0xE: /* 0E PUSH segregs[regcs] */
            push (segregs[regcs]);
            break;

          case 0xF: //0F POP CS only the 8086/8088 does this.
            segregs[regcs] = pop();
            break;
            
          case 0x10:  /* 10 ADC Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_adc8();
            writerm8 (rm, res8);
            break;

          case 0x11:  /* 11 ADC Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_adc16();
            writerm16 (rm, res16);
            break;

          case 0x12:  /* 12 ADC Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_adc8();
            putreg8 (reg, res8);
            break;

          case 0x13:  /* 13 ADC Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_adc16();
            putreg16 (reg, res16);
            break;

          case 0x14:  /* 14 ADC regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_adc8();
            regs.byteregs[regal] = res8;
            break;

          case 0x15:  /* 15 ADC eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_adc16();
            regs.wordregs[regax] = res16;
            break;

          case 0x16:  /* 16 PUSH segregs[regss] */
            push (segregs[regss]);
            break;

          case 0x17:  /* 17 POP segregs[regss] */
            segregs[regss] = pop();
            break;

          case 0x18:  /* 18 SBB Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_sbb8();
            writerm8 (rm, res8);
            break;

          case 0x19:  /* 19 SBB Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_sbb16();
            writerm16 (rm, res16);
            break;

          case 0x1A:  /* 1A SBB Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_sbb8();
            putreg8 (reg, res8);
            break;

          case 0x1B:  /* 1B SBB Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_sbb16();
            putreg16 (reg, res16);
            break;

          case 0x1C:  /* 1C SBB regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_sbb8();
            regs.byteregs[regal] = res8;
            break;

          case 0x1D:  /* 1D SBB eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_sbb16();
            regs.wordregs[regax] = res16;
            break;

          case 0x1E:  /* 1E PUSH segregs[regds] */
            push (segregs[regds]);
            break;

          case 0x1F:  /* 1F POP segregs[regds] */
            segregs[regds] = pop();
            break;

          case 0x20:  /* 20 AND Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_and8();
            writerm8 (rm, res8);
            break;

          case 0x21:  /* 21 AND Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_and16();
            writerm16 (rm, res16);
            break;

          case 0x22:  /* 22 AND Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_and8();
            putreg8 (reg, res8);
            break;

          case 0x23:  /* 23 AND Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_and16();
            putreg16 (reg, res16);
            break;

          case 0x24:  /* 24 AND regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_and8();
            regs.byteregs[regal] = res8;
            break;

          case 0x25:  /* 25 AND eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_and16();
            regs.wordregs[regax] = res16;
            break;

          case 0x27:  /* 27 DAA */
            if ( ( (regs.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
                oper1 = regs.byteregs[regal] + 6;
                regs.byteregs[regal] = oper1 & 255;
                if (oper1 & 0xFF00) {
                    cf = 1;
                  }
                else {
                    cf = 0;
                  }

                af = 1;
              }
            else {
                //af = 0;
              }

            if ( (regs.byteregs[regal]  > 0x9F) || (cf == 1) ) {
                regs.byteregs[regal] = regs.byteregs[regal] + 0x60;
                cf = 1;
              }
            else {
                //cf = 0;
              }

            regs.byteregs[regal] = regs.byteregs[regal] & 255;
            flag_szp8 (regs.byteregs[regal]);
            break;

          case 0x28:  /* 28 SUB Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_sub8();
            writerm8 (rm, res8);
            break;

          case 0x29:  /* 29 SUB Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_sub16();
            writerm16 (rm, res16);
            break;

          case 0x2A:  /* 2A SUB Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_sub8();
            putreg8 (reg, res8);
            break;

          case 0x2B:  /* 2B SUB Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_sub16();
            putreg16 (reg, res16);
            break;

          case 0x2C:  /* 2C SUB regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_sub8();
            regs.byteregs[regal] = res8;
            break;

          case 0x2D:  /* 2D SUB eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_sub16();
            regs.wordregs[regax] = res16;
            break;

          case 0x2F:  /* 2F DAS */
            if ( ( (regs.byteregs[regal] & 15) > 9) || (af == 1) ) {
                oper1 = regs.byteregs[regal] - 6;
                regs.byteregs[regal] = oper1 & 255;
                if (oper1 & 0xFF00) {
                    cf = 1;
                  }
                else {
                    cf = 0;
                  }

                af = 1;
              }
            else {
                af = 0;
              }

            if ( ( (regs.byteregs[regal] & 0xF0) > 0x90) || (cf == 1) ) {
                regs.byteregs[regal] = regs.byteregs[regal] - 0x60;
                cf = 1;
              }
            else {
                cf = 0;
              }

            flag_szp8 (regs.byteregs[regal]);
            break;

          case 0x30:  /* 30 XOR Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            op_xor8();
            writerm8 (rm, res8);
            break;

          case 0x31:  /* 31 XOR Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            op_xor16();
            writerm16 (rm, res16);
            break;

          case 0x32:  /* 32 XOR Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            op_xor8();
            putreg8 (reg, res8);
            break;

          case 0x33:  /* 33 XOR Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            op_xor16();
            putreg16 (reg, res16);
            break;

          case 0x34:  /* 34 XOR regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            op_xor8();
            regs.byteregs[regal] = res8;
            break;

          case 0x35:  /* 35 XOR eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            op_xor16();
            regs.wordregs[regax] = res16;
            break;

          case 0x37:  /* 37 AAA ASCII */
            if ( ( (regs.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
                regs.byteregs[regal] = regs.byteregs[regal] + 6;
                regs.byteregs[regah] = regs.byteregs[regah] + 1;
                af = 1;
                cf = 1;
              }
            else {
                af = 0;
                cf = 0;
              }

            regs.byteregs[regal] = regs.byteregs[regal] & 0xF;
            break;

          case 0x38:  /* 38 CMP Eb Gb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getreg8 (reg);
            flag_sub8 (oper1b, oper2b);
            break;

          case 0x39:  /* 39 CMP Ev Gv */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getreg16 (reg);
            flag_sub16 (oper1, oper2);
            break;

          case 0x3A:  /* 3A CMP Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            flag_sub8 (oper1b, oper2b);
            break;

          case 0x3B:  /* 3B CMP Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            flag_sub16 (oper1, oper2);
            break;

          case 0x3C:  /* 3C CMP regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            flag_sub8 (oper1b, oper2b);
            break;

          case 0x3D:  /* 3D CMP eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            flag_sub16 (oper1, oper2);
            break;

          case 0x3F:  /* 3F AAS ASCII */
            if ( ( (regs.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
                regs.byteregs[regal] = regs.byteregs[regal] - 6;
                regs.byteregs[regah] = regs.byteregs[regah] - 1;
                af = 1;
                cf = 1;
              }
            else {
                af = 0;
                cf = 0;
              }

            regs.byteregs[regal] = regs.byteregs[regal] & 0xF;
            break;

          case 0x40:  /* 40 INC eAX */
            oldcf = cf;
            oper1 = regs.wordregs[regax];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regax] = res16;
            break;

          case 0x41:  /* 41 INC eCX */
            oldcf = cf;
            oper1 = regs.wordregs[regcx];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regcx] = res16;
            break;

          case 0x42:  /* 42 INC eDX */
            oldcf = cf;
            oper1 = regs.wordregs[regdx];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regdx] = res16;
            break;

          case 0x43:  /* 43 INC eBX */
            oldcf = cf;
            oper1 = regs.wordregs[regbx];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regbx] = res16;
            break;

          case 0x44:  /* 44 INC eSP */
            oldcf = cf;
            oper1 = regs.wordregs[regsp];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regsp] = res16;
            break;

          case 0x45:  /* 45 INC eBP */
            oldcf = cf;
            oper1 = regs.wordregs[regbp];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regbp] = res16;
            break;

          case 0x46:  /* 46 INC eSI */
            oldcf = cf;
            oper1 = regs.wordregs[regsi];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regsi] = res16;
            break;

          case 0x47:  /* 47 INC eDI */
            oldcf = cf;
            oper1 = regs.wordregs[regdi];
            oper2 = 1;
            op_add16();
            cf = oldcf;
            regs.wordregs[regdi] = res16;
            break;

          case 0x48:  /* 48 DEC eAX */
            oldcf = cf;
            oper1 = regs.wordregs[regax];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regax] = res16;
            break;

          case 0x49:  /* 49 DEC eCX */
            oldcf = cf;
            oper1 = regs.wordregs[regcx];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regcx] = res16;
            break;

          case 0x4A:  /* 4A DEC eDX */
            oldcf = cf;
            oper1 = regs.wordregs[regdx];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regdx] = res16;
            break;

          case 0x4B:  /* 4B DEC eBX */
            oldcf = cf;
            oper1 = regs.wordregs[regbx];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regbx] = res16;
            break;

          case 0x4C:  /* 4C DEC eSP */
            oldcf = cf;
            oper1 = regs.wordregs[regsp];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regsp] = res16;
            break;

          case 0x4D:  /* 4D DEC eBP */
            oldcf = cf;
            oper1 = regs.wordregs[regbp];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regbp] = res16;
            break;

          case 0x4E:  /* 4E DEC eSI */
            oldcf = cf;
            oper1 = regs.wordregs[regsi];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regsi] = res16;
            break;

          case 0x4F:  /* 4F DEC eDI */
            oldcf = cf;
            oper1 = regs.wordregs[regdi];
            oper2 = 1;
            op_sub16();
            cf = oldcf;
            regs.wordregs[regdi] = res16;
            break;

          case 0x50:  /* 50 PUSH eAX */
            push (regs.wordregs[regax]);
            break;

          case 0x51:  /* 51 PUSH eCX */
            push (regs.wordregs[regcx]);
            break;

          case 0x52:  /* 52 PUSH eDX */
            push (regs.wordregs[regdx]);
            break;

          case 0x53:  /* 53 PUSH eBX */
            push (regs.wordregs[regbx]);
            break;

          case 0x54:  /* 54 PUSH eSP */
            push (regs.wordregs[regsp] - 2);
            break;

          case 0x55:  /* 55 PUSH eBP */
            push (regs.wordregs[regbp]);
            break;

          case 0x56:  /* 56 PUSH eSI */
            push (regs.wordregs[regsi]);
            break;

          case 0x57:  /* 57 PUSH eDI */
            push (regs.wordregs[regdi]);
            break;

          case 0x58:  /* 58 POP eAX */
            regs.wordregs[regax] = pop();
            break;

          case 0x59:  /* 59 POP eCX */
            regs.wordregs[regcx] = pop();
            break;

          case 0x5A:  /* 5A POP eDX */
            regs.wordregs[regdx] = pop();
            break;

          case 0x5B:  /* 5B POP eBX */
            regs.wordregs[regbx] = pop();
            break;

          case 0x5C:  /* 5C POP eSP */
            regs.wordregs[regsp] = pop();
            break;

          case 0x5D:  /* 5D POP eBP */
            regs.wordregs[regbp] = pop();
            break;

          case 0x5E:  /* 5E POP eSI */
            regs.wordregs[regsi] = pop();
            break;

          case 0x5F:  /* 5F POP eDI */
            regs.wordregs[regdi] = pop();
            break;
            
#ifndef CPU_8086
          case 0x60:  /* 60 PUSHA (80186+) */
            oldsp = regs.wordregs[regsp];
            push (regs.wordregs[regax]);
            push (regs.wordregs[regcx]);
            push (regs.wordregs[regdx]);
            push (regs.wordregs[regbx]);
            push (oldsp);
            push (regs.wordregs[regbp]);
            push (regs.wordregs[regsi]);
            push (regs.wordregs[regdi]);
            break;

          case 0x61:  /* 61 POPA (80186+) */
            regs.wordregs[regdi] = pop();
            regs.wordregs[regsi] = pop();
            regs.wordregs[regbp] = pop();
            dummy = pop();
            regs.wordregs[regbx] = pop();
            regs.wordregs[regdx] = pop();
            regs.wordregs[regcx] = pop();
            regs.wordregs[regax] = pop();
            break;

          case 0x62: /* 62 BOUND Gv, Ev (80186+) */
            modregrm();
            getea (rm);
            if (signext32 (getreg16 (reg) ) < signext32 ( getmem16 (ea >> 4, ea & 15) ) ) {
                intcall86 (5); //bounds check exception
              }
            else {
                ea += 2;
                if (signext32 (getreg16 (reg) ) > signext32 ( getmem16 (ea >> 4, ea & 15) ) ) {
                    intcall86(5); //bounds check exception
                  }
              }
            break;

          case 0x68:  /* 68 PUSH Iv (80186+) */
            push (getmem16 (segregs[regcs], ip) );
            StepIP (2);
            break;

          case 0x69:  /* 69 IMUL Gv Ev Iv (80186+) */
            modregrm();
            temp1 = readrm16 (rm);
            temp2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            if ( (temp1 & 0x8000L) == 0x8000L) {
                temp1 = temp1 | 0xFFFF0000L;
              }

            if ( (temp2 & 0x8000L) == 0x8000L) {
                temp2 = temp2 | 0xFFFF0000L;
              }

            temp3 = temp1 * temp2;
            putreg16 (reg, temp3 & 0xFFFFL);
            if (temp3 & 0xFFFF0000L) {
                cf = 1;
                of = 1;
              }
            else {
                cf = 0;
                of = 0;
              }
            break;

          case 0x6A:  /* 6A PUSH Ib (80186+) */
            push (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            break;

          case 0x6B:  /* 6B IMUL Gv Eb Ib (80186+) */
            modregrm();
            temp1 = readrm16 (rm);
            temp2 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if ( (temp1 & 0x8000L) == 0x8000L) {
                temp1 = temp1 | 0xFFFF0000L;
              }

            if ( (temp2 & 0x8000L) == 0x8000L) {
                temp2 = temp2 | 0xFFFF0000L;
              }

            temp3 = temp1 * temp2;
            putreg16 (reg, temp3 & 0xFFFFL);
            if (temp3 & 0xFFFF0000L) {
                cf = 1;
                of = 1;
              }
            else {
                cf = 0;
                of = 0;
              }
            break;

          case 0x6C:  /* 6E INSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            //putmem8 (useseg, regs.wordregs[regsi], portin (regs.wordregs[regdx]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0x6D:  /* 6F INSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            //putmem16 (useseg, regs.wordregs[regsi], portin16 (regs.wordregs[regdx]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0x6E:  /* 6E OUTSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            //portout (regs.wordregs[regdx], getmem8 (useseg, regs.wordregs[regsi]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0x6F:  /* 6F OUTSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            //portout16 (regs.wordregs[regdx], getmem16 (useseg, regs.wordregs[regsi]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;
#endif
          case 0x70:  /* 70 JO Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (of) {
                ip = ip + temp16;
              }
            break;

          case 0x71:  /* 71 JNO Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!of) {
                ip = ip + temp16;
              }
            break;

          case 0x72:  /* 72 JB Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (cf) {
                ip = ip + temp16;
              }
            break;

          case 0x73:  /* 73 JNB Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!cf) {
                ip = ip + temp16;
              }
            break;

          case 0x74:  /* 74 JZ Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (zf) {
                ip = ip + temp16;
              }
            break;

          case 0x75:  /* 75 JNZ Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!zf) {
                ip = ip + temp16;
              }
            break;

          case 0x76:  /* 76 JBE Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (cf || zf) {
                ip = ip + temp16;
              }
            break;

          case 0x77:  /* 77 JA Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!cf && !zf) {
                ip = ip + temp16;
              }
            break;

          case 0x78:  /* 78 JS Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (sf) {
                ip = ip + temp16;
              }
            break;

          case 0x79:  /* 79 JNS Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!sf) {
                ip = ip + temp16;
              }
            break;

          case 0x7A:  /* 7A JPE Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (pf) {
                ip = ip + temp16;
              }
            break;

          case 0x7B:  /* 7B JPO Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!pf) {
                ip = ip + temp16;
              }
            break;

          case 0x7C:  /* 7C JL Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (sf != of) {
                ip = ip + temp16;
              }
            break;

          case 0x7D:  /* 7D JGE Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (sf == of) {
                ip = ip + temp16;
              }
            break;

          case 0x7E:  /* 7E JLE Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if ( (sf != of) || zf) {
                ip = ip + temp16;
              }
            break;

          case 0x7F:  /* 7F JG Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!zf && (sf == of) ) {
                ip = ip + temp16;
              }
            break;

          case 0x80:
          case 0x82:  /* 80/82 GRP1 Eb Ib */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            switch (reg) {
                case 0:
                  op_add8();
                  break;
                case 1:
                  op_or8();
                  break;
                case 2:
                  op_adc8();
                  break;
                case 3:
                  op_sbb8();
                  break;
                case 4:
                  op_and8();
                  break;
                case 5:
                  op_sub8();
                  break;
                case 6:
                  op_xor8();
                  break;
                case 7:
                  flag_sub8 (oper1b, oper2b);
                  break;
                default:
                  break;  /* to avoid compiler warnings */
              }

            if (reg < 7) {
                writerm8 (rm, res8);
              }
            break;

          case 0x81:  /* 81 GRP1 Ev Iv */
          case 0x83:  /* 83 GRP1 Ev Ib */
            modregrm();
            oper1 = readrm16 (rm);
            if (opcode == 0x81) {
                oper2 = getmem16 (segregs[regcs], ip);
                StepIP (2);
              }
            else {
                oper2 = signext (getmem8 (segregs[regcs], ip) );
                StepIP (1);
              }

            switch (reg) {
                case 0:
                  op_add16();
                  break;
                case 1:
                  op_or16();
                  break;
                case 2:
                  op_adc16();
                  break;
                case 3:
                  op_sbb16();
                  break;
                case 4:
                  op_and16();
                  break;
                case 5:
                  op_sub16();
                  break;
                case 6:
                  op_xor16();
                  break;
                case 7:
                  flag_sub16 (oper1, oper2);
                  break;
                default:
                  break;  /* to avoid compiler warnings */
              }

            if (reg < 7) {
                writerm16 (rm, res16);
              }
            break;

          case 0x84:  /* 84 TEST Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            oper2b = readrm8 (rm);
            flag_log8 (oper1b & oper2b);
            break;

          case 0x85:  /* 85 TEST Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            oper2 = readrm16 (rm);
            flag_log16 (oper1 & oper2);
            break;

          case 0x86:  /* 86 XCHG Gb Eb */
            modregrm();
            oper1b = getreg8 (reg);
            putreg8 (reg, readrm8 (rm) );
            writerm8 (rm, oper1b);
            break;

          case 0x87:  /* 87 XCHG Gv Ev */
            modregrm();
            oper1 = getreg16 (reg);
            putreg16 (reg, readrm16 (rm) );
            writerm16 (rm, oper1);
            break;

          case 0x88:  /* 88 MOV Eb Gb */
            modregrm();
            writerm8 (rm, getreg8 (reg) );
            break;

          case 0x89:  /* 89 MOV Ev Gv */
            modregrm();
            writerm16 (rm, getreg16 (reg) );
            break;

          case 0x8A:  /* 8A MOV Gb Eb */
            modregrm();
            putreg8 (reg, readrm8 (rm) );
            break;

          case 0x8B:  /* 8B MOV Gv Ev */
            modregrm();
            putreg16 (reg, readrm16 (rm) );
            break;

          case 0x8C:  /* 8C MOV Ew Sw */
            modregrm();
            writerm16 (rm, getsegreg (reg) );
            break;

          case 0x8D:  /* 8D LEA Gv M */
            modregrm();
            getea (rm);
            putreg16 (reg, ea - segbase (useseg) );
            break;

          case 0x8E:  /* 8E MOV Sw Ew */
            modregrm();
            putsegreg (reg, readrm16 (rm) );
            break;

          case 0x8F:  /* 8F POP Ev */
            modregrm();
            writerm16 (rm, pop() );
            break;

          case 0x90:  /* 90 NOP */
            break;

          case 0x91:  /* 91 XCHG eCX eAX */
            oper1 = regs.wordregs[regcx];
            regs.wordregs[regcx] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x92:  /* 92 XCHG eDX eAX */
            oper1 = regs.wordregs[regdx];
            regs.wordregs[regdx] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x93:  /* 93 XCHG eBX eAX */
            oper1 = regs.wordregs[regbx];
            regs.wordregs[regbx] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x94:  /* 94 XCHG eSP eAX */
            oper1 = regs.wordregs[regsp];
            regs.wordregs[regsp] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x95:  /* 95 XCHG eBP eAX */
            oper1 = regs.wordregs[regbp];
            regs.wordregs[regbp] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x96:  /* 96 XCHG eSI eAX */
            oper1 = regs.wordregs[regsi];
            regs.wordregs[regsi] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x97:  /* 97 XCHG eDI eAX */
            oper1 = regs.wordregs[regdi];
            regs.wordregs[regdi] = regs.wordregs[regax];
            regs.wordregs[regax] = oper1;
            break;

          case 0x98:  /* 98 CBW */
            if ( (regs.byteregs[regal] & 0x80) == 0x80) {
                regs.byteregs[regah] = 0xFF;
              }
            else {
                regs.byteregs[regah] = 0;
              }
            break;

          case 0x99:  /* 99 CWD */
            if ( (regs.byteregs[regah] & 0x80) == 0x80) {
                regs.wordregs[regdx] = 0xFFFF;
              }
            else {
                regs.wordregs[regdx] = 0;
              }
            break;

          case 0x9A:  /* 9A CALL Ap */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            push (segregs[regcs]);
            push (ip);
            ip = oper1;
            segregs[regcs] = oper2;
            break;

          case 0x9B:  /* 9B WAIT */
            break;

          case 0x9C:  /* 9C PUSHF */
            push (makeflagsword() | 0x0800);
            break;

          case 0x9D:  /* 9D POPF */
            temp16 = pop();
            decodeflagsword (temp16);
            break;

          case 0x9E:  /* 9E SAHF */
            decodeflagsword ( (makeflagsword() & 0xFF00) | regs.byteregs[regah]);
            break;

          case 0x9F:  /* 9F LAHF */
            regs.byteregs[regah] = makeflagsword() & 0xFF;
            break;

          case 0xA0:  /* A0 MOV regs.byteregs[regal] Ob */
            regs.byteregs[regal] = getmem8 (useseg, getmem16 (segregs[regcs], ip) );
            StepIP (2);
            break;

          case 0xA1:  /* A1 MOV eAX Ov */
            oper1 = getmem16 (useseg, getmem16 (segregs[regcs], ip) );
            StepIP (2);
            regs.wordregs[regax] = oper1;
            break;

          case 0xA2:  /* A2 MOV Ob regs.byteregs[regal] */
            putmem8 (useseg, getmem16 (segregs[regcs], ip), regs.byteregs[regal]);
            StepIP (2);
            break;

          case 0xA3:  /* A3 MOV Ov eAX */
            putmem16 (useseg, getmem16 (segregs[regcs], ip), regs.wordregs[regax]);
            StepIP (2);
            break;

          case 0xA4:  /* A4 MOVSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            putmem8 (segregs[reges], regs.wordregs[regdi], getmem8 (useseg, regs.wordregs[regsi]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xA5:  /* A5 MOVSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            putmem16 (segregs[reges], regs.wordregs[regdi], getmem16 (useseg, regs.wordregs[regsi]) );
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xA6:  /* A6 CMPSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            oper1b = getmem8 (useseg, regs.wordregs[regsi]);
            oper2b = getmem8 (segregs[reges], regs.wordregs[regdi]);
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 1;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            flag_sub8 (oper1b, oper2b);
            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            if ( (reptype == 1) && !zf) {
                break;
              }
            else if ( (reptype == 2) && (zf == 1) ) {
                break;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xA7:  /* A7 CMPSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            oper1 = getmem16 (useseg,regs.wordregs[regsi]);
            oper2 = getmem16 (segregs[reges], regs.wordregs[regdi]);
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 2;
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            flag_sub16 (oper1, oper2);
            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            if ( (reptype == 1) && !zf) {
                break;
              }

            if ( (reptype == 2) && (zf == 1) ) {
                break;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xA8:  /* A8 TEST regs.byteregs[regal] Ib */
            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            flag_log8 (oper1b & oper2b);
            break;

          case 0xA9:  /* A9 TEST eAX Iv */
            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            flag_log16 (oper1 & oper2);
            break;

          case 0xAA:  /* AA STOSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            putmem8 (segregs[reges], regs.wordregs[regdi], regs.byteregs[regal]);
            if (df) {
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xAB:  /* AB STOSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            putmem16 (segregs[reges], regs.wordregs[regdi], regs.wordregs[regax]);
            if (df) {
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xAC:  /* AC LODSB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            regs.byteregs[regal] = getmem8 (useseg, regs.wordregs[regsi]);
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 1;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xAD:  /* AD LODSW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            oper1 = getmem16 (useseg, regs.wordregs[regsi]);
            regs.wordregs[regax] = oper1;
            if (df) {
                regs.wordregs[regsi] = regs.wordregs[regsi] - 2;
              }
            else {
                regs.wordregs[regsi] = regs.wordregs[regsi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xAE:  /* AE SCASB */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            oper1b = regs.byteregs[regal];
            oper2b = getmem8 (segregs[reges], regs.wordregs[regdi]);
            flag_sub8 (oper1b, oper2b);
            if (df) {
                regs.wordregs[regdi] = regs.wordregs[regdi] - 1;
              }
            else {
                regs.wordregs[regdi] = regs.wordregs[regdi] + 1;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            if ( (reptype == 1) && !zf) {
                break;
              }
            else if ( (reptype == 2) && (zf == 1) ) {
                break;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xAF:  /* AF SCASW */
            if (reptype && (regs.wordregs[regcx] == 0) ) {
                break;
              }

            oper1 = regs.wordregs[regax];
            oper2 = getmem16 (segregs[reges], regs.wordregs[regdi]);
            flag_sub16 (oper1, oper2);
            if (df) {
                regs.wordregs[regdi] = regs.wordregs[regdi] - 2;
              }
            else {
                regs.wordregs[regdi] = regs.wordregs[regdi] + 2;
              }

            if (reptype) {
                regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
              }

            if ( (reptype == 1) && !zf) {
                break;
              }
            else if ( (reptype == 2) & (zf == 1) ) {
                break;
              }

            totalexec++;
            loopcount++;
            if (!reptype) {
                break;
              }

            ip = firstip;
            break;

          case 0xB0:  /* B0 MOV regs.byteregs[regal] Ib */
            regs.byteregs[regal] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB1:  /* B1 MOV regs.byteregs[regcl] Ib */
            regs.byteregs[regcl] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB2:  /* B2 MOV regs.byteregs[regdl] Ib */
            regs.byteregs[regdl] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB3:  /* B3 MOV regs.byteregs[regbl] Ib */
            regs.byteregs[regbl] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB4:  /* B4 MOV regs.byteregs[regah] Ib */
            regs.byteregs[regah] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB5:  /* B5 MOV regs.byteregs[regch] Ib */
            regs.byteregs[regch] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB6:  /* B6 MOV regs.byteregs[regdh] Ib */
            regs.byteregs[regdh] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB7:  /* B7 MOV regs.byteregs[regbh] Ib */
            regs.byteregs[regbh] = getmem8 (segregs[regcs], ip);
            StepIP (1);
            break;

          case 0xB8:  /* B8 MOV eAX Iv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            regs.wordregs[regax] = oper1;
            break;

          case 0xB9:  /* B9 MOV eCX Iv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            regs.wordregs[regcx] = oper1;
            break;

          case 0xBA:  /* BA MOV eDX Iv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            regs.wordregs[regdx] = oper1;
            break;

          case 0xBB:  /* BB MOV eBX Iv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            regs.wordregs[regbx] = oper1;
            break;

          case 0xBC:  /* BC MOV eSP Iv */
            regs.wordregs[regsp] = getmem16 (segregs[regcs], ip);
            StepIP (2);
            break;

          case 0xBD:  /* BD MOV eBP Iv */
            regs.wordregs[regbp] = getmem16 (segregs[regcs], ip);
            StepIP (2);
            break;

          case 0xBE:  /* BE MOV eSI Iv */
            regs.wordregs[regsi] = getmem16 (segregs[regcs], ip);
            StepIP (2);
            break;

          case 0xBF:  /* BF MOV eDI Iv */
            regs.wordregs[regdi] = getmem16 (segregs[regcs], ip);
            StepIP (2);
            break;

          case 0xC0:  /* C0 GRP2 byte imm8 (80186+) */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            writerm8 (rm, op_grp2_8 (oper2b) );
            break;

          case 0xC1:  /* C1 GRP2 word imm8 (80186+) */
            modregrm();
            oper1 = readrm16 (rm);
            oper2 = getmem8 (segregs[regcs], ip);
            StepIP (1);
            writerm16 (rm, op_grp2_16 ( (byte) oper2) );
            break;

          case 0xC2:  /* C2 RET Iw */
            oper1 = getmem16 (segregs[regcs], ip);
            ip = pop();
            regs.wordregs[regsp] = regs.wordregs[regsp] + oper1;
            break;

          case 0xC3:  /* C3 RET */
            ip = pop();
            break;

          case 0xC4:  /* C4 LES Gv Mp */
            modregrm();
            getea (rm);
            putreg16 (reg, read86 (ea) + read86 (ea + 1) * 256);
            segregs[reges] = read86 (ea + 2) + read86 (ea + 3) * 256;
            break;

          case 0xC5:  /* C5 LDS Gv Mp */
            modregrm();
            getea (rm);
            putreg16 (reg, read86 (ea) + read86 (ea + 1) * 256);
            segregs[regds] = read86 (ea + 2) + read86 (ea + 3) * 256;
            break;

          case 0xC6:  /* C6 MOV Eb Ib */
            modregrm();
            writerm8 (rm, getmem8 (segregs[regcs], ip) );
            StepIP (1);
            break;

          case 0xC7:  /* C7 MOV Ev Iv */
            modregrm();
            writerm16 (rm, getmem16 (segregs[regcs], ip) );
            StepIP (2);
            break;

          case 0xC8:  /* C8 ENTER (80186+) */
            stacksize = getmem16 (segregs[regcs], ip);
            StepIP (2);
            nestlev = getmem8 (segregs[regcs], ip);
            StepIP (1);
            push (regs.wordregs[regbp]);
            frametemp = regs.wordregs[regsp];
            if (nestlev) {
                for (temp16 = 1; temp16 < nestlev; temp16++) {
                    regs.wordregs[regbp] = regs.wordregs[regbp] - 2;
                    push (regs.wordregs[regbp]);
                  }

                push (regs.wordregs[regsp]);
              }

            regs.wordregs[regbp] = frametemp;
            regs.wordregs[regsp] = regs.wordregs[regbp] - stacksize;

            break;

          case 0xC9:  /* C9 LEAVE (80186+) */
            regs.wordregs[regsp] = regs.wordregs[regbp];
            regs.wordregs[regbp] = pop();
            break;

          case 0xCA:  /* CA RETF Iw */
            oper1 = getmem16 (segregs[regcs], ip);
            ip = pop();
            segregs[regcs] = pop();
            regs.wordregs[regsp] = regs.wordregs[regsp] + oper1;
            break;

          case 0xCB:  /* CB RETF */
            ip = pop();;
            segregs[regcs] = pop();
            break;

          case 0xCC:  /* CC INT 3 */
            intcall86 (3);
            break;

          case 0xCD:  /* CD INT Ib */
            oper1b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            intcall86 (oper1b);
            break;

          case 0xCE:  /* CE INTO */
            if (of) {
                intcall86 (4);
              }
            break;

          case 0xCF:  /* CF IRET */
            ip = pop();
            segregs[regcs] = pop();
            decodeflagsword (pop() );

            /*
             * if (net.enabled) net.canrecv = 1;
             */
            break;

          case 0xD0:  /* D0 GRP2 Eb 1 */
            modregrm();
            oper1b = readrm8 (rm);
            writerm8 (rm, op_grp2_8 (1) );
            break;

          case 0xD1:  /* D1 GRP2 Ev 1 */
            modregrm();
            oper1 = readrm16 (rm);
            writerm16 (rm, op_grp2_16 (1) );
            break;

          case 0xD2:  /* D2 GRP2 Eb regs.byteregs[regcl] */
            modregrm();
            oper1b = readrm8 (rm);
            writerm8 (rm, op_grp2_8 (regs.byteregs[regcl]) );
            break;

          case 0xD3:  /* D3 GRP2 Ev regs.byteregs[regcl] */
            modregrm();
            oper1 = readrm16 (rm);
            writerm16 (rm, op_grp2_16 (regs.byteregs[regcl]) );
            break;

          case 0xD4:  /* D4 AAM I0 */
            oper1 = getmem8 (segregs[regcs], ip);
            StepIP (1);
            if (!oper1) {
                intcall86 (0);
                break;
              } /* division by zero */

            regs.byteregs[regah] = (regs.byteregs[regal] / oper1) & 255;
            regs.byteregs[regal] = (regs.byteregs[regal] % oper1) & 255;
            flag_szp16 (regs.wordregs[regax]);
            break;

          case 0xD5:  /* D5 AAD I0 */
            oper1 = getmem8 (segregs[regcs], ip);
            StepIP (1);
            regs.byteregs[regal] = (regs.byteregs[regah] * oper1 + regs.byteregs[regal]) & 255;
            regs.byteregs[regah] = 0;
            flag_szp16 (regs.byteregs[regah] * oper1 + regs.byteregs[regal]);
            sf = 0;
            break;

          case 0xD6:  /* D6 XLAT on V20/V30, SALC on 8086/8088 */
            regs.byteregs[regal] = cf ? 0xFF : 0x00;
            break;

          case 0xD7:  /* D7 XLAT */
            regs.byteregs[regal] = read86(useseg * 16 + (regs.wordregs[regbx]) + regs.byteregs[regal]);
            break;

          case 0xD8:
          case 0xD9:
          case 0xDA:
          case 0xDB:
          case 0xDC:
          case 0xDE:
          case 0xDD:
          case 0xDF:  /* escape to x87 FPU (unsupported) */
            modregrm();
            break;

          case 0xE0:  /* E0 LOOPNZ Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
            if ( (regs.wordregs[regcx]) && !zf) {
                ip = ip + temp16;
              }
            break;

          case 0xE1:  /* E1 LOOPZ Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
            if (regs.wordregs[regcx] && (zf == 1) ) {
                ip = ip + temp16;
              }
            break;

          case 0xE2:  /* E2 LOOP Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            regs.wordregs[regcx] = regs.wordregs[regcx] - 1;
            if (regs.wordregs[regcx]) {
                ip = ip + temp16;
              }
            break;

          case 0xE3:  /* E3 JCXZ Jb */
            temp16 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            if (!regs.wordregs[regcx]) {
                ip = ip + temp16;
              }
            break;

          case 0xE4:  /* E4 IN regs.byteregs[regal] Ib */
            oper1b = getmem8 (segregs[regcs], ip);
            StepIP (1);
           // regs.byteregs[regal] = (byte) portin (oper1b);
            break;

          case 0xE5:  /* E5 IN eAX Ib */
            oper1b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            //regs.wordregs[regax] = portin16 (oper1b);
            break;

          case 0xE6:  /* E6 OUT Ib regs.byteregs[regal] */
            oper1b = getmem8 (segregs[regcs], ip);
            StepIP (1);
            //portout (oper1b, regs.byteregs[regal]);
            break;

          case 0xE7:  /* E7 OUT Ib eAX */
            oper1b = getmem8 (segregs[regcs], ip);
            StepIP (1);
           //portout16 (oper1b, regs.wordregs[regax]);
            break;

          case 0xE8:  /* E8 CALL Jv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            push (ip);
            ip = ip + oper1;
            break;

          case 0xE9:  /* E9 JMP Jv */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            ip = ip + oper1;
            break;

          case 0xEA:  /* EA JMP Ap */
            oper1 = getmem16 (segregs[regcs], ip);
            StepIP (2);
            oper2 = getmem16 (segregs[regcs], ip);
            ip = oper1;
            segregs[regcs] = oper2;
            break;

          case 0xEB:  /* EB JMP Jb */
            oper1 = signext (getmem8 (segregs[regcs], ip) );
            StepIP (1);
            ip = ip + oper1;
            break;

          case 0xEC:  /* EC IN regs.byteregs[regal] regdx */
            oper1 = regs.wordregs[regdx];
           // regs.byteregs[regal] = (byte) portin (oper1);
            break;

          case 0xED:  /* ED IN eAX regdx */
            oper1 = regs.wordregs[regdx];
           // regs.wordregs[regax] = portin16 (oper1);
            break;

          case 0xEE:  /* EE OUT regdx regs.byteregs[regal] */
            oper1 = regs.wordregs[regdx];
            //portout (oper1, regs.byteregs[regal]);
            break;

          case 0xEF:  /* EF OUT regdx eAX */
            oper1 = regs.wordregs[regdx];
            //portout16 (oper1, regs.wordregs[regax]);
            break;

          case 0xF0:  /* F0 LOCK */
            break;

          case 0xF4:  /* F4 HLT */
            hltstate = 1;
            break;

          case 0xF5:  /* F5 CMC */
            if (!cf) {
                cf = 1;
              }
            else {
                cf = 0;
              }
            break;

          case 0xF6:  /* F6 GRP3a Eb */
            modregrm();
            oper1b = readrm8 (rm);
            op_grp3_8();
            if ( (reg > 1) && (reg < 4) ) {
                writerm8 (rm, res8);
              }
            break;

          case 0xF7:  /* F7 GRP3b Ev */
            modregrm();
            oper1 = readrm16 (rm);
            op_grp3_16();
            if ( (reg > 1) && (reg < 4) ) {
                writerm16 (rm, res16);
              }
            break;

          case 0xF8:  /* F8 CLC */
            cf = 0;
            break;

          case 0xF9:  /* F9 STC */
            cf = 1;
            break;

          case 0xFA:  /* FA CLI */
            ifl = 0;
            break;

          case 0xFB:  /* FB STI */
            ifl = 1;
            break;

          case 0xFC:  /* FC CLD */
            df = 0;
            break;

          case 0xFD:  /* FD STD */
            df = 1;
            break;

          case 0xFE:  /* FE GRP4 Eb */
            modregrm();
            oper1b = readrm8 (rm);
            oper2b = 1;
            if (!reg) {
                tempcf = cf;
                res8 = oper1b + oper2b;
                flag_add8 (oper1b, oper2b);
                cf = tempcf;
                writerm8 (rm, res8);
              }
            else {
                tempcf = cf;
                res8 = oper1b - oper2b;
                flag_sub8 (oper1b, oper2b);
                cf = tempcf;
                writerm8 (rm, res8);
              }
            break;

          case 0xFF:  /* FF GRP5 Ev */
            modregrm();
            oper1 = readrm16 (rm);
            op_grp5();
            break;

          default:
            break;
        }

skipexecution:
      if (!running) {
          return;
        }
    }
}
