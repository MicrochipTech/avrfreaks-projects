program FixedPointMath;
// demonstration program for AFP lib (ARBITRARY FIXED POINT MATH LIBRARY)

{$NOSHADOW}
{ $WG}                     {global Warnings off}

Device = mega128, VCC=5;
{ $BOOTRST $0F000}         {Reset Jump to $0F000}

Import SysTick, SerPort, TWIMaster, LCDMultiPort;

From System Import LongInt, Float;


Define
  ProcClock    = 16000000;       {Hertz}
  SysTick      = 10;             {msec}
  StackSize    = $0064, iData;
  FrameSize    = $00C8, iData;
  SerPort      = 9600, Stop1;    {Baud, StopBits|Parity}
  RxBuffer     = 8, iData;
  TxBuffer     = 8, iData;
  TWIpresc     = TWI_BR400;
  LCDmultiPort = I2C_TWI;
  LCDrows_M    = 2;              {rows}
  LCDcolumns_M = 40;             {columns per line}
  LCDtype_M    = 44780;
  DecimalSep   = ',';

uses
  Fix;    // import fixed point math library
  
Implementation

{$IDATA}

{--------------------------------------------------------------}
{ Type Declarations }

type

{--------------------------------------------------------------}
{ Const Declarations }

{--------------------------------------------------------------}
{ Var Declarations }
{$IDATA}
var
  i, j, teta: TFix;
  TmpStr: string[40];

{--------------------------------------------------------------}
{ functions }


{--------------------------------------------------------------}
{ Main Program }
{$IDATA}

begin
  EnableInts;
  LCDsetup_M(LCD_m1);
  LCDsetup_M(LCD_m2);
  LCDsetup_M(LCD_m3);
  LCDsetup_M(LCD_m4);
  LCDsetup_M(LCD_m5);
  LCDsetup_M(LCD_m6);
  LCDsetup_M(LCD_m7);
  LCDsetup_M(LCD_m8);

  fixInit(16); // try 10, 16, 20 or some other number of bits that you want for fractional part
  //
  LCDcursor_M(LCD_m7, false, false);
  LCDxy_M(LCD_m7, 0, 0);
  Write(LCDout_M, ' 1.0:  ' + FixToStr(LongToFix(1)) + ' ' + LongToHex(LongToFix(1)));
  LCDxy_M(LCD_m7, 0, 1);
  Write(LCDout_M, '-1.0: ' + FixToStr(LongToFix(-1)) + ' ' + LongToHex(LongToFix(-1)));
  LCDcursor_M(LCD_m8, false, false);
  LCDxy_M(LCD_m8, 0, 0);
  Write(LCDout_M, ' 1.1:  ' + FixToStr(FloatToFix(1.1)) + ' ' + LongToHex(FloatToFix(1.1)));
  LCDxy_M(LCD_m8, 0, 1);
  Write(LCDout_M, '-1.1: ' + FixToStr(FloatToFix(-1.1)) + ' ' + LongToHex(FloatToFix(-1.1)));
  //
  LCDcursor_M(LCD_m5, false, false);
  LCDxy_M(LCD_m5, 0, 0);
  //Write(LCDout_M, ' 5.0 * -1  : ' + FixToStr(FixMulLong(FloatToFix(5.0), -1)) + ' ' + LongToHex(FixMulLong(FloatToFix(5.0), -1)));
  Write(LCDout_M, ' 5.0 * -1  : ' + FixToStr(FloatToFix(5.0) * (-1)) + ' ' + LongToHex(FloatToFix(5.0) * (-1)));
  LCDxy_M(LCD_m5, 0, 1);
  Write(LCDout_M, ' 1.0 * -1.0: ' + FixToStr(FixMul(FloatToFix(1.0), FloatToFix(-1.0))) + ' ' + LongToHex(FixMul(FloatToFix(1.0), FloatToFix(-1.0))));
  LCDcursor_M(LCD_m6, false, false);
  LCDxy_M(LCD_m6, 0, 0);
  Write(LCDout_M, '-2.5 * -2.5:  ' + FixToStr(FixMul(FloatToFix(-2.5), FloatToFix(-2.5))) + ' ' + LongToHex(FixMul(FloatToFix(-2.5), FloatToFix(-2.5))) + ' SQRT:'+ FixToStr(FixSqrt(FloatToFix(6.25))));
  LCDxy_M(LCD_m6, 0, 1);
  Write(LCDout_M, '-2.5 -  7.4: ' + FixToStr(FloatToFix(-2.5) - FloatToFix(7.4)) + ' ' + LongToHex(FloatToFix(-2.5) - FloatToFix(7.4)));
  //
  teta := IntToFix(-720);
  //
  //i := fixOne; // 1.000
  //i := i * 100000; // for integer multiplication FixNum * i = fixMul(FixNum,LongIntToFix(i)), but first is faster :-)
  i := fixPI;
  //i := fixGetOne * (-1);
  //i := %10000000 00000001 00000000 00000000;
  //i := fixSub(i, fixOne); // Believe it or not, this is the same as i:=i-fixOne, without any change in compiler :-)
  j := 0;
  LCDcursor_M(LCD_m1, false, false);
  LCDxy_M(LCD_m1, 0, 0);
  Write(LCDout_M, 'fixOne:' + LongToStr(fixOne));
  LCDxy_M(LCD_m1, 15, 0);
  //Write(LCDout_M, 'fixBits: ' + ByteToStr(fixGetFractBits));
  Write(LCDout_M, 'fractDig:' + ByteToStr(fixGetFractDigits));
  loop
    LCDxy_M(LCD_m1, 26, 0);
    //Write(LCDout_M, 'i: ' + LongToStr(i));
    Write(LCDout_M, 'i:' + FixToStr(i));
    //Write(LCDout_M, 'i:' + fixGetFractPartAsString(i));
    LCDxy_M(LCD_m1,0, 1);
    Write(LCDout_M, 'i: ' + IntToBin(HiWord(i)) + ' ' + IntToBin(LoWord(i)));
    //Write(LCDout_M, 'i: ' + FloatToStr(FixToFloat(i)));
    //Write(LCDout_M, FloatToStr(FixToFloat(Min(TFix))) + ' ' + FloatToStr(FixToFloat(Max(TFix))));
    //Write(LCDout_M, FloatToStr(FixToFloat(fixOne) / 100) + ' ' + FloatToStr(FixToFloat(FloatToFix(3.14))));
    //Write(LCDout_M, FloatToStr(FixToFloat(fixOne)) + ' ' + FloatToStr(FixToFloat(fixOne) / 1000 * FixToFloat((LongIntToFix(1000)))));
    //
    LCDcursor_M(LCD_m3, false, false);
    //LCDCLR_M(LCD_m3);
    LCDxy_M(LCD_m3, 0, 0);
    Write(LCDout_M, 'angle=' + FixToStr(teta) + ' sinfloat=' + FloatToStr(SinD(FixToFloat(teta))) + '  ');
    LCDxy_M(LCD_m3, 0, 1);
    Write(LCDout_M, 'sin=' + FixToStr(fixSinD(teta)) + ' cos=' + FixToStr(fixCosD(teta)) + ' tan=' + FixToStr(fixDiv(fixSinD(teta), fixCosD(teta))) + ' Q=' + ByteToStr(fixQuadrant(teta, angleDeg)) + '  ');
    //
    LCDcursor_M(LCD_m2, false, false);
    LCDxy_M(LCD_m2, 0, 0);
    Write(LCDout_M, 'j: ' + FixToStr(j) + ' ' + LongToHex(j) + '  -j: ' + FixToStr(-j) + ' ' + LongToHex(-j) + '  ');
    LCDxy_M(LCD_m2, 0, 1);
    Write(LCDout_M, FixToStr(j) + ' + ' + FixToStr(-j) + ' = ' + FixToStr(fixAdd(j, -j)) + ' ' + LongToHex(fixAdd(j, -j)) + '  ');
    //
    LCDcursor_M(LCD_m4, false, false);
    LCDxy_M(LCD_m4, 0, 0);
    Write(LCDout_M, FixToStr(fixOne+j) + ' ' + LongToHex(fixOne+j) + '    ' + FixToStr(fixOne-j) + ' ' + LongToHex(fixOne-j));
    //Write(LCDout_M, 'fixSinD=' + FixToStr(fixSinD(teta)) + ' fixSin=' + FixToStr(FixSin(fixDegToRad(teta))) + '  ');
    LCDxy_M(LCD_m4, 0, 1);
    Write(LCDout_M, FixToStr(fixOne) + '*' + LongToStr(j) + '=' + FixToStr(fixOne*j) + '   ' + FixToStr(fixOne) + '/' + LongToStr(j) + '=' + FixToStr(fixDivLong(fixOne,j)) + ' ');
    //
    teta := teta + IntToFix(30); // nice place for a breakpoint :-)
    j := j + 1;
    //i := fixAdd(i, fixOne);
    //i := i - 1;
    i := i + 1;
    //i := i - FloatToFix(0.1); // place breakpoint here, and run loop 20 times to see accumulation of error and understand everything
    //i := i + FloatToFix(0.1);
    //i := fixMul(i, FloatToFix(-2.5));
    //i := fixMul(i, FloatToFix(-0.5));
    //i := fixDiv(i, FloatToFix(-4)); // place breapoint here
  endloop;
end FixedPointMath.

