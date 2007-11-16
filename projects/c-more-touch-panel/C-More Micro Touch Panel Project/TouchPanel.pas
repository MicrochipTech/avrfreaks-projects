///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// AvrCo demo project for C-More Micro touch panel (model EA1-S3ML)                  //
// You need to download associated project ModbusAVR.mgp to C-More Micro touch panel //
// MODBUS RTU communication is used to monitor and control AVR with this touch panel //
//                                                                                   //
// Zeljko Avramovic (c) 2007, avra at writeme dot com                                //
// Source is free for any use, but you take full responsibility for using it         //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////

program TouchPanel;

{ $BOOTRST $0F000}         {Reset Jump to $0F000}
{ $WG}                     {global Warnings off}

Device = mega128, VCC=5;

Import SysTick, SerPort, LCDport, BeepPort, Watchdog, Modbus;
From System  Import LongWord, LongInt, Float, Processes, Pipes;
From SysTick Import SystemTime32;

Define
  ProcClock      = 16000000;
  SysTick        = 10;
  StackSize      = $0100, iData;
  FrameSize      = $0100, iData;
  Scheduler      = iData;
  SerPort        = 38400, Databit8, parEven, Stop1;
  RxBuffer       = 10, iData;
  TxBuffer       = 64, iData;
  ModBus         = SerPort, 40, iData, 240;
  ModBusMode     = RTU, Timer3;
  LCDport        = PortA;  // LCD is needed just for your
  LCDtype        = 44780;  // better understanding.
  LCDrows        = 2;      // You don't need it really
  LCDcolumns     = 16;     // for this demo to work.
  BeepPort       = PortG, 4;
  WatchDog       = msec1000;
  
uses
  ModBusServRTU;

Implementation

const
  ON: char  = #6;
  OFF: char = #5;

{$IDATA}
type
  tPanelKey           = (F1=0, F2=1, F3=2, F4=3, F5=4, ESC=8, MENU=9, CLR=10, ENT=11,
                         LEFT_ARROW=12, RIGHT_ARROW=13, UP_ARROW=14, DOWN_ARROW=15);
  tSwitchesAndButtons = (SW1=0, SW2=1, SW3=2, SW4=3, SW5=4, SW6=5, SW7=6, SW8=7,
                         BT1=8, BT2=9, BT3=10, BT4=11, BT5=12, BT6=13, BT7=14, BT8=15);
  tSwitchAndButtonSet = BitSet of tSwitchesAndButtons;
  
var
  i: byte;
  scans: longword;
  Previous: boolean;
  SecTimer: SysTimer;
  LcdRow1, LcdRow2: string[16];
  
  {$MODBUS ModbusBuffer}
  MB[@ModDPR] : record                                        // word byte
                  Alarms            : mb_InpW;                // [00] 00..01
                  DummyWord0        : mb_InpW;                // [01] 02..03
                  DummyByte0        : mb_RdWrB;               // [02] 04     this is lo(ModDPR[2])
                  DummyByte1        : mb_RdWrB;               // [02] 05     this is hi(ModDPR[2])
                  CmoreScreenJump   : mb_RdWrW;               // [03] 06..07 set shown screen number
                  CmoreBeep         : mb_RdWrW;               // [04] 08..09 enable/disable and set beeper
                  CmoreColor        : mb_RdWrW;               // [05] 10..11 change background color
                  DummyString0      : array[0..1] of char;    // [06] 12..13 only multiple of 2 possible, always R/W
                  MultiStateMsg     : mb_InpW;                // [07] 14..15 what message to show on screen 5
                  ScansPerSecond    : mb_InpW32;              // [08] 16..19 calculated AVR speed
                  VersionText       : array[1..10] of char;   // [10] 20..29
                  DummyWord1        : mb_InpW;                // [15] 30..31
                  DummyWord2        : mb_InpW;                // [16] 32..33
                  DummyWord3        : mb_InpW;                // [17] 34..35
                  DummyWord4        : mb_InpW;                // [18] 36..37
                  DummyWord5        : mb_InpW;                // [19] 38..39
                  FloatEdit         : mb_InpF;                // [20] 40..43
                  IntegerEdit       : mb_InpI;                // [22] 44..45 visual bar graph like editor
                  CmoreScreenNumber : mb_InpW;                // [23] 46..47 current screen number
                  CmoreKeyPressed1  : mb_InpW;                // [24] 48..49 pressed key from keyset 1
                  CmoreKeyPressed2  : mb_InpW;                // [25] 50..51 pressed key from keyset 2
                  DummyWord6        : mb_InpW;                // [26] 52..53
                  Switches          : tSwitchAndButtonSet;    // [27] 54..55
                  DummyLong0        : mb_InpW32;              // [28] 56..59
                  DummyLong1        : mb_InpW32;              // [30] 60..63
                  DummyLong2        : mb_RdWrW32;             // [32] 64..67
                  DummyLong3        : mb_RdWrW32;             // [34] 68..71
                  DummyWord7        : mb_RdWrW;               // [36] 72..73
                  DummyWord8        : mb_RdWrW;               // [37] 74..75
                  DummyLong4        : mb_RdWrW32;             // [38] 76..79
                end;

  PanelKey    [@MB.CmoreKeyPressed1]: tPanelKey;
  FloatHex    [@MB.FloatEdit]: LongWord;
  PushButtons [@MB.Switches+1]: Byte;

  // There is no official MODBUS standard if numbers are zero-based or one-based. Comunication is
  // always zero-based, but addressing in user interface for specific device may be zero or one-based.
  // Some devices (like C-More) use one-based (just in user interface!), so everything seams shifted
  // compared to AvrCo (bits and words). Do not let this confuse you. That's the way it is. :-(

procedure Init;
begin
  DDRA := %11111111;
  PortA := 0;
  DDRD := 255;
  PortD := 255;

  // User defined characters for displaying ON/OFF bits
  LCDCharSet(OFF, $1F, $11, $11, $11, $11, $11, $11, $1F);
  LCDCharSet(ON,  $1F, $1F, $1F, $1F, $1F, $1F, $1F, $1F);

  // Init MODBUS tags
  Setbit(MB.CmoreBeep.15, 1); // enable Cmore beeper
  Incl(MB.Switches, [SW5]);   // set ON first radio button in screen 4
  MB.VersionText := StrToArr('AVR Str!'); // It's up to you to make sure there are <= 9 chars! (10 chars array - 1 for zero terminator)
  MB.FloatEdit := 283.14;
end;


procedure mb_BeforeCoilWrite(CoilNumber: word; var NewValue: boolean);
begin  // MODBUS driver callback function
  if ValueInRange(CoilNumber, 436, 439) then // only for 4 radio buttons
    Excl(MB.Switches, [SW5]);
    Excl(MB.Switches, [SW6]);
    Excl(MB.Switches, [SW7]);
    Excl(MB.Switches, [SW8]);
  endif;
end;


procedure RefreshLCD;
begin
  LCDClr;
  LCDxy(0,0);

  case MB.CmoreScreenNumber of
    1:    LcdRow1 := 'Alarms=';
          for i := 0 to 4 do
            if Bit(MB.Alarms, i) then
              LcdRow1 := LcdRow1 + Char(ON);
            else
              LcdRow1 := LcdRow1 + Char(OFF);
            endif;
          endfor;
          if Bit(MB.Alarms, 2) then
            LcdRow2 := 'Critical alarm!';
          elsif Bit(MB.Alarms, 4) then
            LcdRow2 := 'Tick alarm!';
          else
            LcdRow2 := '';
          endif;|
    2:    LcdRow1 := 'Scans='  + LongToStr(MB.ScansPerSecond);
          LcdRow2 := 'String=' + ArrToStr(MB.VersionText);| // if alarm5 is on then string will be shown on C-More display
    3:    LcdRow1 := 'Alarms=' + ByteToBin(Lo(MB.Alarms)); // editable on C-More
          LcdRow2 := 'Float='  + FloatToStr(MB.FloatEdit:7:2);| // editable on C-More
    4:    LcdRow1 := 'Switch=' + ByteToBin(LoNibble(Lo(MB.Switches))); // set in C-More
          LcdRow2 := 'Radio='  + ByteToBin(HiNibble(Lo(MB.Switches)));| // set in C-More
    5:    LcdRow1 := 'Pushbuttons= '; // set in C-More
          for i := 0 to 2 do
            if Bit(PushButtons, i) then
              LcdRow1 := LcdRow1 + Char(ON);
            else
              LcdRow1 := LcdRow1 + Char(OFF);
            endif;
          endfor;
          LcdRow2 := 'IncMsgPush3=' + IntToStr(MB.MultiStateMsg);| // incremented on each PUSH3 button press
  endcase;
  //LcdRow2 := 'Key=' + IntToStr(MB.CmoreKeyPressed1:5:0); // you can monitor C-More buttons

  Write(LCDout, LcdRow1);
  LCDxy(0,1);
  Write(LCDout, LcdRow2);
end;


process Work(16, 16: iData);
begin
  MB.IntegerEdit := ValueTrimLimit(MB.IntegerEdit, 0, 5); // C-More can increment it more then we would like
  MB.Alarms := 1 Rol (Byte(MB.IntegerEdit) - 1);
  
  if (not Previous) and PushButtons.2 then
    Previous := true;
    MB.MultiStateMsg := (MB.MultiStateMsg + 1) mod 4;
  endif;

  if Previous and (not PushButtons.2) then
    Previous := false;
  endif;
  
  Schedule;
end;

{--------------------------------------------------------------}
{ Main Program }
{$IDATA}
begin
  Init;
  mb_SetBeforeCoilWrite(@mb_BeforeCoilWrite); // initialize user's MODBUS event and enable it
  Start_Processes;
  
  loop
    Inc(scans);
    if isSysTimerZero(SecTimer) then
      SetSysTimer(SecTimer, 1000 div SysTick); // 1 second
      MB.ScansPerSecond := scans;
      RefreshLCD;
      scans := 0;
      if Bit(MB.Alarms, 4) then
        BeepClick;
      endif;
    endif;
    WatchDogTrig;
  endloop;
end TouchPanel.

