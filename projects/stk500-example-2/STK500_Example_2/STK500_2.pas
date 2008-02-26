/////////////////////////////////////////////////////////////////
//                                                             //
//  STK500 Example 2                     (c) Zeljko Avramovic  //
//                                                avra@ptt.yu  //
//  26.07.2002.                     http://compocontrol.da.ru  //
//                                                             //
//                                                             //
//  1) 4 switches for songs, 4 switches for sounds.            //
//  2) Songs are in process, sounds are in main, so try to     //
//     press switch for song and then switch for sound. You    //
//     can hear one song and one sound at the same time.       //
//                                                             //
//  Feel free to use it or improve it in any way you want.     //
//                                                             //
//  There is nothing here specific to STK500, so feel free to  //
//  try it even if you don't have STK500. You may even try it  //
//  with another processor or frequency.                       //
//                                                             //
/////////////////////////////////////////////////////////////////


program STK500_2;

{$NOSHADOW}

Device = 90S8515;

Import SysTick, BeepPort, SwitchPort1;

From System Import Processes;

Define
        ProcClock   = 3686400;        {Hertz}
        SysTick     = 1;              {msec}
        StackSize   = $0015, iData;
        FrameSize   = $0010, iData;
        Scheduler   = iData;
        BeepPort    = PortD, 6;
        SwitchPort1 = PinA, $FF;
        PolarityP1  = $00;            // polarity

Implementation

{$IDATA}

{--------------------------------------------------------------}
{ Type Declarations }

type

{--------------------------------------------------------------}
{ Const Declarations }
const
 		  BeepLen: Byte = 100 div SysTick;

{--------------------------------------------------------------}
{ Var Declarations }
{$IDATA}
var
	Switches: byte;
	Leds: byte;
	Sw0[@Switches, 0]: bit;
	Sw1[@Switches, 1]: bit;
	Sw2[@Switches, 2]: bit;
	Sw3[@Switches, 3]: bit;
	Sw4[@Switches, 4]: bit;
	Sw5[@Switches, 5]: bit;
	Sw6[@Switches, 6]: bit;
	Sw7[@Switches, 7]: bit;

{--------------------------------------------------------------}
{ functions }
procedure InitPorts;
begin
  PortB := %11111111;
  DDRB  := %11111111;
end InitPorts;

procedure PlayMusic(const s : string[120]);
var
  x      : byte;
  power  : word;
  octave : word;
begin
  octave := 2;
  power := 4;
  for x := 1 to length(s) do
    case s[x] of
      'c': BeepOut(65  * power + octave, BeepLen);|
      'C': BeepOut(69  * power + octave, BeepLen);|
      'd': BeepOut(73  * power + octave, BeepLen);|
      'D': BeepOut(78  * power + octave, BeepLen);|
      'e': BeepOut(82  * power + octave, BeepLen);|
      'f': BeepOut(87  * power + octave, BeepLen);|
      'F': BeepOut(92  * power + octave, BeepLen);|
      'g': BeepOut(98  * power + octave, BeepLen);|
      'G': BeepOut(104 * power + octave, BeepLen);|
      'a': BeepOut(110 * power + octave, BeepLen);|
      'A': BeepOut(116 * power + octave, BeepLen);|
      'b': BeepOut(123 * power + octave, BeepLen);|
      ' ': BeepOut(0, BeepLen);|
      '0': power := 1;  octave := 0;|  // 2^0
      '1': power := 2;  octave := 1;|	// 2^1
      '2': power := 4;  octave := 2;|	// 2^2
      '3': power := 8;  octave := 3;|	// 2^3
      '4': power := 16; octave := 4;|	// 2^4
    else
		BeepOut(35, BeepLen*5);
    endcase;
  endfor;
end;

process Songs(32, 140: iData);
begin
	if Bit(Sw7) then
		SetBit(Leds, 7, false);
      // Jingle Bells:
      PlayMusic('3e e e   e e e   e ');
      PlayMusic('3g c d ee   f f f   ');
      PlayMusic('3e e e   d d d e d ');
      PlayMusic('3g   e e e   e e e   ');
      PlayMusic('3e g c d e   f f f   ');
      PlayMusic('3e e e   g g e d cc');
      // End of Jingle Bells
      SetBit(Leds, 7, true);
   endif;

	if Bit(Sw6) then
		SetBit(Leds, 6, false);
      // Beverly Hills Cop
      PlayMusic('2FFFFaa FFFAAFFee');
      PlayMusic('2FFFF3cc 2FFF3ddCC');
      PlayMusic('2aaFF3CCFF2FeeeCC');
      PlayMusic('2GGFFFF  ');
      // End of Beverly Hills Cop
		SetBit(Leds, 6, true);
	endif;

	if Bit(Sw5) then
		SetBit(Leds, 5, false);
      // Yesterday:
      PlayMusic('3ggffffff          ');
      PlayMusic('3aabb2CCddeeffeeeddddddd          ');
      PlayMusic('4ddddcc1AAaaggAAAAAaaaaaaa ');
      PlayMusic('3ggggffffaaggggg ddddffffaaaaaaaa');
      // End of Yesterday
		SetBit(Leds, 5, true);
	endif;

	if Bit(Sw4) then
		SetBit(Leds, 4, false);
      // Star Wars:
    	PlayMusic('2dd  dd  dd  1AA 2fdd');
      PlayMusic('2  1AA 2fdddd    aa  aa');
      PlayMusic('2  aa  AA fC  1AA 2fdddd');
      PlayMusic('2    3dd  2dd d3dd  CC c');
      PlayMusic('2aAaa  D GG  gg Ffeff  ');
      PlayMusic('1A 2CC1AA 2Cff  dd faaaa');
      PlayMusic('3    dd  2dd d3dd  3CC ');
      PlayMusic('3c2aAaa  D GG  gg Ffeff');
      PlayMusic('1  A 2CC  1AA 2fdd  1AA ');
      PlayMusic('2fdddd  ');
      // End of Star Wars
      //
      // Entertainer:
    	// PlayMusic('3dDe4cc3e4cc3e4cccc');
      // PlayMusic('4  cdDecdee3b4ddcccc');
      // End of Entertainer
		SetBit(Leds, 4, true);
	endif;

end;

{--------------------------------------------------------------}
{ Main Program }
{$IDATA}

begin
	InitPorts;
	Leds := $FF;

	Start_Processes;

	loop
		Switches := Port_Stable1;

      if Bit(Sw0) then
		   Toggle(Leds, 0);
			PortB := Leds;
   	   BeepSiren(0);
		   Toggle(Leds, 0);
      endif;

      if Bit(Sw1) then
		   Toggle(Leds, 1);
			PortB := Leds;
   	   BeepSiren(1);
		   Toggle(Leds, 1);
      endif;

      if Bit(Sw2) then
		   Toggle(Leds, 2);
			PortB := Leds;
   	   BeepOutHL;
		   Toggle(Leds, 2);
      endif;

      if Bit(Sw3) then
		   Toggle(Leds, 3);
			PortB := Leds;
   	   BeepOutLH;
		   Toggle(Leds, 3);
      endif;

		PortB := Leds;

	endloop;
end STK500_2.
