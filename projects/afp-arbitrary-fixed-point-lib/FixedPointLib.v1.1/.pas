Unit Fix;

///////////////////////////////////////////////
//                                           //
//               === AFP ===                 //
//                                           //
//    ARBITRARY FIXED POINT MATH LIBRARY     //
//                                           //
//         Zeljko Avramovic (c) 2009         //
//                                           //
//          avra at writeme dot com          //
//                                           //
//      www.encodeteam.com/resume.html       //
//                                           //
///////////////////////////////////////////////

// HISTORY:
//
// v0.1 Initial Release
// v0.2 Fixed bug in FixToStr()
// v0.3 Fixed bugs in fixGetFractPart(), fixMul(), fixDiv()
// v0.4 Implemented Merlin's suggestions, added fixSin(), fixCos(), fixRadToDeg(), fixDegToRad(), fixGetPI()
// v0.5 Improved fixDiv(), added fixDivInt(), fixDivLong(). Other small changes.
// v0.6 Improved code size - Sin() and Cos() share the same code
// v0.7 Added fixCosD(), fixSinD(), fixMod(), fixModInt()
// v0.8 Small code size and speed optimizations, exposed math constants to the public
// v0.9 Added fixSin() and fixCos() now support any angle
// v1.0 Fixed fixSin() and fixCos() calculation, cleaned code a little
// v1.1 Added fixSqrt()


interface
// global part

{ $W+}                  // enable/disable warnings for this unit

uses ;

{--------------------------------------------------------------}
{ Const Declarations }
const
  FIX_MAX_STRING_LENGTH: byte = 12; // max length of all strings that contain fixed point numbers

{$IDATA}
{--------------------------------------------------------------}
{ Type Declarations }
type
  TFix = LongInt;          // use 32 bits for Fixed Point Math type
  TFixFractBits = byte;    // fix for subrange not yet supported in AvrCo
  TFixFractDigits = byte;
  TFixString = string[FIX_MAX_STRING_LENGTH]; // strings containing fixed point numbers
  TaylorApproximation = (taylorSin, taylorCos); // sine or cosine
  AngleType = (angleDeg, angleRad); // angle in degrees or radians
{--------------------------------------------------------------}
{ Var Declarations }
var
  fixOne: TFix; // number neutral in multiplication and division
  fixOneHalf, fixOneQuater, fixOneThreeFourths: TFix;
  fixPI: TFix; // we have to calculate it in runtime because of unknown number of fractional bits
  fixPIHALF, {fixPIQUATER,} fixTWOPI: TFix;   // precalculate to speed up calculations
  {fix45, }fix90, fix180, fix360: TFix; // precalculate to speed up calculations
  fixD2R, fixR2D: TFix; // precalculate to speed up calculations

function  fixGetFractBits: TFixFractBits;
procedure fixSetFractBits(const NumberOfFractBits: TFixFractBits);
//function  fixGetOne: TFix;
//function  fixGetPI: TFix;
function  fixGetFractDigits: TFixFractDigits;
function  fixGetFractMultiplier: longint;
procedure fixInit(const NumberOfFractBits: TFixFractBits);
function  fixAdd(const a, b: TFix): TFix;
function  fixSub(const a, b: TFix): TFix;
function  fixMul(const a, b: TFix): TFix;
function  fixDiv(const a, b: TFix): TFix;
function  fixMulLong(const a: TFix; const b: longint): TFix;
function  fixMulInt(const a: TFix; const b: integer): TFix;
function  fixDivLong(const a: TFix; const b: longint): TFix;
function  fixDivInt(const a: TFix; const b: integer): TFix;
// conversion functions:
function  LongToFix(const a: longint): TFix;
function  FixToLong(const a: TFix): longint;
function  IntToFix(const a: integer): TFix;
function  FixToInt(const a: TFix): integer;
{$IFDEF float} // this helps us not forcing users to import float if they don't use it
function  FloatToFix(const a: float): TFix;
function  FixToFloat(const a: TFix): float;
{$ENDIF }
function  FixToStr(const a: TFix): TFixString;
function  fixGetIntPart(const a: TFix): longint;
function  fixGetFractPart(a: TFix): longint;
function  fixGetFractDigitsOnce: TFixFractDigits;
function  fixGetPIOnce: TFix;
function  fixGetIntPartAsString(const a: TFix): TFixString;
function  fixGetFractPartAsString(const a: TFix): TFixString;
function  fixPow10Long(a: byte): longint;
// trigonometry
function fixSin(const radians: TFix): TFix;
function fixCos(const radians: TFix): TFix;
function fixSinD(const degrees: TFix): TFix;
function fixCosD(const degrees: TFix): TFix;
function fixSinCos(const radians: TFix; const SinOrCos: TaylorApproximation): TFix;
function fixRadToDeg(const rad: TFix): TFix;
function fixDegToRad(const deg: TFix): TFix;
function fixQuadrant(const angle: TFix; const RadOrDeg: AngleType): byte;
function fixMod(const a, modulus: TFix): TFix;
function fixModInt(const a: TFix; modulus: integer): TFix;

function fixSqrt(a: TFix): TFix;

{--------------------------------------------------------------}
{ functions }


implementation
// local part

{--------------------------------------------------------------}
{ Type Declarations }
type

{--------------------------------------------------------------}
{ Const Declarations }
const
  FIX_MIN_NUMBER_OF_INTEGER_BITS:   TFixFractBits = 5; // restriction because of PI calculation on initialization (to represent 22: as TFix we need minimum 5 bits)
  FIX_MIN_NUMBER_OF_FRACT_BITS:     TFixFractBits = 0;
  FIX_MAX_NUMBER_OF_BITS:           TFixFractBits = 31;
  FIX_MAX_NUMBER_OF_FRACT_BITS:     TFixFractBits = FIX_MAX_NUMBER_OF_BITS - FIX_MIN_NUMBER_OF_INTEGER_BITS;
  FIX_DEFAULT_NUMBER_OF_FRACT_BITS: TFixFractBits = 10;
  // TFix default number of bits (s21.10): fractional part is 2^10 = 1024, and integer part is 2^21 = 2097152
  // S-IIIIIIIIIIIIIIIIIIIII-FFFFFFFFFF (1 bit for sign, 21 bits for integer part, 10 bits for fractional part
  // TFix most common number of bits (s15.16): fractional part is 2^16 = 65536, and integer part is 2^15 = 32768
  // S-IIIIIIIIIIIIIII-FFFFFFFFFFFFFFFF (1 bit for sign, 15 bits for integer part, 16 bits for fractional part

  FIX_MASK_FOR_SIGN_BIT: longint =        %10000000 00000000 00000000 00000000;

{--------------------------------------------------------------}
{ Var Declarations }
{$IDATA}
var
  fixFractBits: TFixFractBits; // TFix: 1 bit for sign, fixFractBits for fractional part, the rest for integer part
  fixFractDigits: TFixFractDigits; // number of decimals in fractional part
                                   // fixFractBits=10 => fixOne=1024 => TFix=+-IIIIIII.999
  fixFractMultiplier: longint; // multiply fractional part with this number to get decimal presentation as integer
                               // example: fractional part is 1023 and fixOne=1023 => decimal part = 1023/1024 = 0.999
                               // 0.999 * FractMultiplier = 999
  fixMaskForFractPart: TFix; // needed to mask just bits of fractional part of fixed point numbers

{--------------------------------------------------------------}
{ functions }
function fixGetFractBits: TFixFractBits;
begin // get number of fractional bits in all fixed point math numbers
  Return (fixFractBits);
end;

procedure fixSetFractBits(const NumberOfFractBits: TFixFractBits);
begin // set number of fractional bits in all fixed point math numbers
  fixFractBits        := ValueTrimLimit(NumberOfFractBits, FIX_MIN_NUMBER_OF_FRACT_BITS, FIX_MAX_NUMBER_OF_FRACT_BITS);
  fixOne              := 1 shl fixFractBits; // we have to adjust neutral whenever number of fractional bits is changed
  fixOneHalf          := fixOne shr 1;               // fixOne/2
  fixOneQuater        := fixOneHalf shr 1;           // fixOne/4
  fixOneThreeFourths  := fixOneHalf + fixOneQuater;  // 3*fixOne/4
  fixMaskForFractPart := fixOne - 1;
  fixFractDigits      := fixGetFractDigitsOnce;
  fixFractMultiplier  := fixPow10Long(fixFractDigits);
  fixPI               := fixGetPIOnce;
  fixTWOPI            := fixPI shl 1; // this is the same as fixPI * 2 but saves 16 bytes
  fixPIHALF           := fixPI shr 1; // this is 12 bytes less flash space then fixPI div 2
  //fixPIQUATER         := fixPIHALF shr 1;
  //fix45             := 45  shl fixFractBits;
  fix90               := 90  shl fixFractBits; // the same as IntToFix(90) but saves 2 flash bytes :-)
  fix180              := 180 shl fixFractBits;
  fix360              := 360 shl fixFractBits;
  fixR2D              := fixDiv(fix180, fixPI);
  fixD2R              := fixDivInt(fixPI, 180);
end;

{function  fixGetOne: TFix;
begin
  Return(fixOne);
end;}

function  fixGetPIOnce: TFix;
begin // find suitable PI approximation with highest number of correct fractional digits (PI = 3.14159265358979323846264338327950288…)
  if (FIX_MAX_NUMBER_OF_BITS - fixFractBits) > 9 then // we need at least 9 bits for 355
    Return(fixDivInt(IntToFix(355), 113)); // use PI approximation with 6 correct fractional digits (without rounding)
  else // we need at least 5 bits for 22 (FIX_MIN_NUMBER_OF_INTEGER_BITS is already 5 so this is ensured)
    Return(fixDivInt(IntToFix(22), 7)); // PI approximation with just 2 correct fractional digits (without rounding)
  endif;
end;

{function  fixGetPI: TFix;
begin
  Return(fixPI);
end;}

function  fixGetFractDigits: TFixFractDigits;
begin
  Return(fixFractDigits);
end;

function  fixGetFractMultiplier: longint;
begin
  Return(fixFractMultiplier);
end;

procedure fixInit(const NumberOfFractBits: TFixFractBits);
begin
  fixSetFractBits(NumberOfFractBits);
end;

function fixAdd(const a, b: TFix): TFix;
begin // add a and b with fixed point math
  Return (a + b);
end;

function fixSub(const a, b: TFix): TFix;
begin // subtract b from a with fixed point math
  Return (a - b);
end;

function fixMul(const a, b: TFix): TFix;
begin // multiply a and b with fixed point math
  //Return ((a * b) shr fixFractBits);
  Return (longint((int64(a) * int64(b)) shr fixFractBits));
end;

function fixMulLong(const a: TFix; const b: longint): TFix;
begin // multiply a and b with fixed point math
  Return (a * b);
end;

function fixMulInt(const a: TFix; const b: integer): TFix;
begin // multiply a and b with fixed point math
  Return (a * longint(b));
end;

function fixDiv(const a, b: TFix): TFix;
begin // divide a with b without rounding fixed point math
  //Return ((a shl fixFractBits) div b);
  Return (longint((int64(a) shl fixFractBits) div int64(b)));
  //Return (longint((int64(a + (abs(b) shra 1)) shl fixFractBits) div int64(b))); // rounding ???
end;

function fixDivLong(const a: TFix; const b: longint): TFix;
begin // divide a with b without rounding fixed point math
  Return (a div b);
  //Return ((a + (abs(b) shra 1)) div b); // rounding ???
end;

function fixDivInt(const a: TFix; const b: integer): TFix;
begin // divide a with b without rounding fixed point math
  Return (a  div longint(b));
  //Return ((a + longint(abs(b) shra 1)) div longint(b)); // rounding ???
end;

function LongToFix(const a: longint): TFix;
begin
  Return (a shla fixFractBits);
end;

function FixToLong(const a: TFix): longint;
begin
  Return (a shra fixFractBits);
end;

function IntToFix(const a: integer): TFix;
begin
  Return (LongToFix(longint(a)));
end;

function FixToInt(const a: TFix): integer;
begin
  Return (integer(FixToLong(a)));
end;

{$IFDEF float} // this helps us not forcing users to import float if they don't use it
function FloatToFix(const a: float): TFix;
begin
  Return (Trunc(a * float(fixOne)));
end;

function FixToFloat(const a: TFix): float;
begin
  Return (float(a) / float(fixOne));
end;
{$ENDIF}

function FixToStr(const a: TFix): TFixString;
var
  FixStr, TmpStr: TFixString;
begin
  TmpStr := fixGetIntPartAsString(a);
  if (a < 0) and (TmpStr[1] <> '-') then // bugfix for special case negative numbers between 0 and -1
    FixStr := '-';
  else
    FixStr := '';
  endif;
  // Append (src : string; var dst : string); // this might speed up a little - no tests yet
  FixStr := FixStr + TmpStr + Char(DecimalSep); //using Char(DecimalSep) instead of just DecimalSep solved the bug
  FixStr := FixStr + fixGetFractPartAsString(a);
  Return (FixStr);
end;

function fixGetIntPart(const a: TFix): longint;
begin
  Return (FixToLong(a));
end;

function fixGetIntPartAsString(const a: TFix): TFixString;
var
  FixStr: TFixString;
begin
  FixStr := LongToStr(fixGetIntPart(a));
  Return (FixStr);
end;

function fixGetFractPart(a: TFix): longint;
begin
  if a < 0 then
    a := (not a) + 1;
  endif;
  a := a and fixMaskForFractPart; // the same as a := a and (fixOne - 1) but faster
  //a := a * fixFractMultiplier;
  //a := a div fixOne;
  a := MulDivLong(a, fixFractMultiplier, fixOne); // without this change we were limited with 16 bits for fractional part
  Return (a);
  // Return (((a and (fixOne - 1)) * fixFractMultiplier) div fixOne);
  //Return (longint((int64(a) * int64(fixFractMultiplier)) div int64(fixOne)));
end;

function fixGetFractPartAsString(const a: TFix): TFixString;
var
  FixStr, TmpStr: TFixString;
begin
  TmpStr := LongToStr(fixGetFractPart(a));
  FixStr := PadLeft(TmpStr, fixFractDigits, '0');
  Return (FixStr);
end;

function fixGetFractDigitsOnce: TFixFractDigits;
var
  FractDigits: TFixFractDigits;{ = 0} // this compiles but doesn't work without explicit assignment;
  TmpFixOne: longint;
begin
  FractDigits := 0;
  TmpFixOne := fixOne; // 1024 if default 10 bits for fractional part
  while TmpFixOne > 9 do
    inc(FractDigits);
    TmpFixOne := TmpFixOne div 10;
  endwhile;
  Return (FractDigits);
end;

function  fixPow10Long(a: byte): longint;
var // this is not for fixed point numbers - this deals only with longint since AvrCo Pow10() supports only floats
  TmpPower: longint;
begin
  TmpPower := 1;
  while a > 0 do
    TmpPower := TmpPower * 10;
    Dec(a);
  endwhile;
  Return (TmpPower);
end;

{function fixSinRaw(const rad: TFix): TFix;
var
  x, lasts, s, fact, num: TFix;
  opsign: integer;
  i: byte;
begin
  x := rad;
  i := 1;
  lasts := 0;
  s := x;
  fact := 1;
  num := x;
  opsign := 1;
  while s <> lasts do
    lasts := s;
    Inc(i, 2);
    fact := fixMulLong(fact, LongInt(i * (i-1)));
    num := fixMul(num, fixMul(x, x));
    opsign := opsign * (-1);
    s := s + fixDivLong((num * longint(opsign)), fact); //s := s + (num / fact * opsign);
    //
    LCDcursor_M(LCD_m2, false, false);
    LCDCLR_M(LCD_m2);
    //LCDxy_M(LCD_m2, 0, 0);
    Write(LcdOut_M, 'lasts=' + FixToStr(lasts) + ' i=' + ByteToStr(i) + ' fact=' + FixToStr(fact));
    LCDxy_M(LCD_m2, 0, 1);
    Write(LcdOut_M, 'num=' + FixToStr(num) + ' opsign=' + IntToStr(opsign) + ' s=' + FixToStr(s));
    LCDcursor_M(LCD_m4, false, false);
    LCDCLR_M(LCD_m4);
    Write(LcdOut_M, '(num * opsign)=' + FixToStr(num * longint(opsign)));
    LCDxy_M(LCD_m4, 0, 1);
    Write(LcdOut_M, 'fixDivLong((num * opsign),fact)=' + FixToStr(fixDivLong((num * longint(opsign)), fact)));
    LCDcursor_M(LCD_m1, false, false);
  endwhile;
  Return(s);
end;}

function fixSinCos(const rad: TFix; const SinOrCos: TaylorApproximation): TFix;
var // Taylor series sine/cosine approximation method for {-PI..PI}, en.wikipedia.org/wiki/Taylor_series
  x, lasts, s, num: TFix;
  //opsign, i: integer;
  opsign, i, fact: longint;
begin
  x      := rad;
  lasts  := 0;
  fact   := 1;
  opsign := 1;
  
  if SinOrCos = taylorSin then  // sine
    i   := 1;
    s   := x;
    num := x;
  else       // taylorCos       // cosine
    i   := 0;
    s   := fixOne;
    num := fixOne;
  endif;
  
  while s <> lasts do
    lasts  := s;
    Inc(i, 2);
    //fact   := fixMulLong(fact, LongInt(i * (i-1)));
    fact   := fact * i * (i-1);
    num    := fixMul(num, fixMul(x, x));
    //opsign := opsign * (-1);
    opsign := -opsign; // speed optimization avoids multiplication with -1
    //s      := s + fixDivLong((num * longint(opsign)), fact);
    //Inc(s, fixDivLong(num * opsign, fact)); //s := s + (num / fact * opsign);
    //
    // optimize speed in num * opsign by avoiding multiplication with opsign:
    if opsign > 0 then  // opsign =  1
      Inc(s, fixDivLong(num, fact)); //s := s + (num / fact * opsign);
    else                // opsign = -1
      Inc(s, fixDivLong(-num, fact)); //s := s + (num / fact * opsign);
    endif;
  endwhile;
  Return(ValueTrimLimit(s, -fixOne, fixOne));
end;

function fixSin(const rad: TFix): TFix;
var
  quadrant: byte;
  taySin, tayCos: taylorApproximation;
  Tmp: TFix;
begin
  if rad < 0 then
    tayCos := taylorCos;
    taySin := taylorSin;
  else
    tayCos := taylorSin;
    taySin := taylorCos;
  endif;
  quadrant := fixQuadrant(rad, angleRad); // determine quadrant to know what sign we will need at the end
  if not ValueInRange(rad, 0, fixPIHALF) then
    rad := abs(fixMod(rad, fixPIHALF));
  endif;
  //Tmp := rad;// just for printing
  //LCDcursor_M(LCD_m8, false, false);
  //LCDCLR_M(LCD_m8);
  //Write(LcdOut_M, 'deg0..90=' + FixToStr(fixRadToDeg(rad)) + ' deg=' + FixToStr(fixRadToDeg(Tmp)) + ' rad=' + FixToStr(rad) + '  ');
  //LCDxy_M(LCD_m8, 0, 1);
  //Write(LcdOut_M, 'tSin=' + FixToStr(fixSinCos(rad, taylorSin)) + ' tCos=' + FixToStr(fixSinCos(rad, taylorCos)) + ' Q=' + ByteToStr(quadrant) + '  ');
  //LCDcursor_M(LCD_m4, false, false);
  case quadrant of
    1: Tmp :=  fixSinCos(rad, tayCos);|
    2: Tmp :=  fixSinCos(rad, taySin);|
    3: Tmp := -fixSinCos(rad, tayCos);|
  else // 4
       Tmp := -fixSinCos(rad, taySin);
  endcase;
  Return(Tmp);
end;

function fixSinD(const deg: TFix): TFix;
begin
  Return(fixSin(fixDegToRad(deg)));
end;

function fixCos(const rad: TFix): TFix;
begin
  Return(fixSin(fixPIHALF - rad));
end;

function fixCosD(const deg: TFix): TFix;
begin
  Return(fixCos(fixDegToRad(deg)));
end;

function fixRadToDeg(const rad: TFix): TFix;
begin
  //Return (fixDiv(rad * 180, fixPI));
  Return(fixMul(rad, fixR2D));
end;

function fixDegToRad(const rad: TFix): TFix;
begin
  //Return (fixMul(rad, fixPI) div 180);
  Return(fixMul(rad, fixD2R));
end;

function fixQuadrant(const angle: TFix; const RadOrDeg: AngleType): byte;
var // determine quadrant of an angle
  Tmp: TFix;
begin
  if RadOrDeg = angleRad then
    Tmp := fixTWOPI;
  else
    Tmp := fix360;
  endif;
  Tmp := fixDiv(angle, Tmp) and fixMaskForFractPart; // just get fractional part (maybe later replace DIV with MUL)
  if Tmp < fixOneQuater then // after anding it is already positive, so we just ask if it is less then 1/4
    Return(1);
  else
    if Tmp < fixOneHalf then // after anding it is already positive, so we just ask if it is less then 1/4
      Return(2);
    else
      if Tmp < fixOneThreeFourths then // after anding it is already positive, so we just ask if it is less then 1/4
        Return(3);
      else
        Return(4);
      endif;
    endif;
  endif;
end;

{function fixQuadrantD(const deg: TFix): byte;
begin // determine quadrant for angle in degrees
  if (deg >= 0) then
    Return(byte(FixToInt(fixModInt(fixDiv(deg, fix90), 4) + fixOne)));   // ((deg / 90) % 4) + 1);
  else
    Return(byte(4 - byte(FixToInt(fixModInt(fixDiv(-deg, fix90), 4))))); // (4 - (-deg / fix90) % 4))
  endif;
end;}

function fixSqrt(a: TFix): TFix;
var // basically this is an integer square root calculation, but we need it since AvrCo Sqrt() works only with float
  result, m, tmp : longint;
begin
  m := $40000000;
  result := 0;
  while m <> 0 do
    tmp := m + result;
    if a >= tmp then
      Dec(a, tmp);
      result := (result shr 1) or m;
    else
      result := result shr 1;
    endif;
    m := m shr 2;
  endwhile;
  return(result shl (fixFractBits shr 1)); // correct decimal point to the right place
end;

function fixModInt(const a: TFix; modulus: integer): TFix;
begin // a mod modulus   Example: fixModInt(-361.2345, 360) = -1.2345
  Return(a mod IntToFix(modulus));
end;

function fixMod(const a, modulus: TFix): TFix;
begin // a mod modulus   Example: fixMod(-361.2345, 360.0000) = -1.2345
  Return(a mod modulus);
end;

initialization
// at StartUp
  //fixSetFractBits(FIX_DEFAULT_NUMBER_OF_FRACT_BITS);
  
// finalization          // optional
// at System_ShutDown
end Fix.

