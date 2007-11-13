/////////////////////////////////////////////////////////////////
//                                                             //
//  STK500 Example 1                     (c) Zeljko Avramovic  //
//                                                avra@ptt.yu  //
//  25.07.2002.                     http://compocontrol.da.ru  //
//                                                             //
//                                                             //
//  1) Write instructions on COM port                          //
//  2) Move flashing led to left/right on left/right key press //
//  3) Beep on every move, write move to COM port              //
//  4) Lit left/right direction led                            //
//  5) Store flashing led position to eeprom                   //
//                                                             //
//  Feel free to use it or improve it in any way you want.     //
//                                                             //
//  There is nothing here specific to STK500, so feel free to  //
//  try it even if you don't have STK500. You may even try it  //
//  with another processor or frequency.                       //
//                                                             //
/////////////////////////////////////////////////////////////////


program STK500_1;

{$NOSHADOW}
{ $W+ Warnings}            {Warnings off}

Device = 90S8515, VCC=5;

Import SysTick, SwitchPort1, SerPort, BeepPort;

From System Import ;

Define
        ProcClock   = 3686400;        {Maximum frequency with STK500 built in oscilator}
        SysTick     = 5;              {msec}
        StackSize   = $0020, iData;
        FrameSize   = $0010, iData;
        SwitchPort1 = PinA;           // connect switches to port A, connect leds to port B
        PolarityP1  = $00;            // polarity
        SerPort     = 9600, Stop1;
        TxBuffer    = 32, iData;
        //RxBuffer    = 32, iData;
        BeepPort    = PortD, 6;       // connect piezo beeper to PD6 and GND. Use 1uF condensator as described in SoundDrv.PDF


Implementation

{$IDATA}

{--------------------------------------------------------------}
{ Type Declarations }

type


{--------------------------------------------------------------}
{ Const Declarations }
const
   RIGHT: Byte                    = 0;
   LEFT:  Byte                    = 7;
   FirstFlashingLedPosition: Byte = 1;
   LastFlashingLedPosition:  Byte = 6;
   RightDirectionLed: Byte        = RIGHT;
   LeftDirectionLed: Byte         = LEFT;

{--------------------------------------------------------------}
{ Var Declarations }
{$IDATA}
var
   Leds: Byte;                                   // Prepare Leds for PortB
   RightDirectionKey[@Port_Stable1, RIGHT]: bit; // PortB.0 moves flashing led to the right
   LeftDirectionKey[@Port_Stable1, LEFT]: bit;   // PortB.7 moves flashing led to the left
{$EEPROM}
   Dummy: word;                                  // Some AVRs have problems with first two bytes
   FlashingLedPosition: Byte;                    // store position of flashing led in eeprom - PortB.FlashingLedPosition
{--------------------------------------------------------------}
{ functions }

procedure InitPorts;
begin
  PortB := %11111111;
  DDRB  := %11111111;
end InitPorts;


{--------------------------------------------------------------}
{ Main Program }
{$IDATA}

begin
  InitPorts;
  Leds := $FF;

  FlashingLedPosition := Within(1, FlashingLedPosition, 6);  // Make sure that FlashingLedPosition has valid value
                                                             // This way, old saved value is loaded (if valid)
  EnableInts;

  Write(SerOut, #13 + #10 + 'Instructions: Press keys 0 and 7 to move flashing led!' + #13 + #10 + #13 + #10);

  loop
      SetBit(Leds, RightDirectionLed, not Bit(RightDirectionKey));  // If right key is pressed, lit right direction led
      SetBit(Leds, LeftDirectionLed, not Bit(LeftDirectionKey));    // If left key is pressed, lit left direction led

      // process right move:
      if not Bit(Leds, RightDirectionLed) then
         if (FlashingLedPosition > FirstFlashingLedPosition) then
            FlashingLedPosition := FlashingLedPosition - 1;
            Write(SerOut, 'COMMAND: >>>    POSITION: ' + ByteToStr(FlashingLedPosition) + #13 + #10);
            //BeepSiren(1);
            //BeepOutLH;
            BeepOut(2000, 2);
         else
            BeepOut(50,10);   // Error!
         endif;
      endif;

      // process left move:
      if not Bit(Leds, LeftDirectionLed) then
         if (FlashingLedPosition < LastFlashingLedPosition) then
            FlashingLedPosition := FlashingLedPosition + 1;
            Write(SerOut, 'COMMAND: <<<    POSITION: ' + ByteToStr(FlashingLedPosition) + #13 + #10);
            //BeepSiren(2);
            //BeepOutHL;
            BeepOut(2000, 2);
         else
            BeepOut(50,10);   // Error!
         endif;
      endif;

      Leds := not Leds;
      Leds := Leds and %10000001;
      Leds := not Leds;

      SetBit(Leds, FlashingLedPosition, not Bit(Leds, FlashingLedPosition));
      mDelay(50);
      PortB := Leds;

      SetBit(Leds, FlashingLedPosition, not Bit(Leds, FlashingLedPosition));
      mDelay(100);
      PortB := Leds;
  endloop;
end STK500_1.
