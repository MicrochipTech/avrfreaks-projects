(*

-----------------------------------------------------------------------------------------
                                     STATE
-----------------------------------------------------------------------------------------
 THIS SOFTWARE IS FREEWARE AND IS PROVIDED AS IS AND COMES WITH NO WARRANTY OF ANY
 KIND, EITHER EXPRESSED OR IMPLIED. IN NO EVENT WILL THE AUTHOR(S) BE LIABLE FOR
 ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE.
-----------------------------------------------------------------------------------------
                                  INFORMATION
-----------------------------------------------------------------------------------------
 Description : btBeeper uses the PC speaker to produce various sounds.
               In fact you can make it play entire songs.

               It contains many "preset" sounds. You can play them
               just calling the PlayPresetSound method.

               Also it has the ability to play songs or sounds
               written in a plain ascii file.

               I wrote it just for fun but I think it maybe usefull
               in some situations ie. when a sound card is not present
               or when you don't like to use .wav files

               The code is straightforward and well commented - I think

               I used some fucntions posted to a (don't remember) newsgroup
               by jatkins@paktel.compulink.co.uk (John Atkins)
 Tested      : Delphi 5 , Win2K
 Author      : Theo Bebekis <bebekis@otenet.gr> 
 More info   :
 License     : Freeware
 Thanks to   : John Atkins [jatkins@paktel.compulink.co.uk]
-----------------------------------------------------------------------------------------
                                     HISTORY
-----------------------------------------------------------------------------------------
 Version   Date          Changes - Additions                                By
-----------------------------------------------------------------------------------------
 0.01      25.09.1988    Initial Version                                    
 0.02      24.10.1988    ElapsedMillisecFrom function
                         The ElapsedMillisecFrom function added as a
                         correction to BeepFor function. ElapsedMillisecFrom
                         posted to me by John Herbster (johnh@petronworld.com)
                         as a solution to GetTickCount Win API function
                         problem. Beeper uses GetTickCount to calculate
                         the time for beep durations
 0.03      26.10.1988    1. FBeeping boolean field added to prevent calling a
                            beeping function again while a beep is currently
                            played
                         2. Application.ProcessMessages call removed from the
                            BeepFor function's while loop to prevent undesired
                            sound effects if the owner form is receiving
                            moving messages or a new form is created modally
                            while a beep is played at the same time
 0.04      21.05.2001    Added TSoundThread and the helper classes
 0.05      27.05.2001    Added BeepSequence method for playing sequences of sounds
                         The BeepSequence method provided by
                         Friedrich Ing. Hajny [Cum_Hajny@compuserve.com]
 0.06      08.01.2002    ElapsedMillisecFrom and the waiting loop replaced by
                         simple calls to Sleep() as suggested by
                         Peter Williams [pew@pcug.org.au]
-----------------------------------------------------------------------------------------
   
*)


unit btOdeum;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;

const
 { WinApi Help denotes that frequency for the PC speaker (the dwFreq parameter for
   the Windows Beep function ) must be in the range 37 through 32,767 (0x25 through 0x7FFF).
   Well, I think that frequency > 5000 is to hard and reedy. The pitch is to high for the PC speaker.
   In addition I founded easier to use preset frequencies as musical tones  - see below - than
   directly use a particular frequency.
   Of course the human ear can distinguish sounds lesser than a half step, so you always the chance
   to use such intervals when calling TbtBeeper methods }
 LOW_FREQ   = 40;
 HIGH_FREQ  = 5000;

 { Denotes a pseudo - frequency for the rests }
 REST = 1;

 { I use five octaves with TbtBeeper. C_0, C, C_1, C_2 and C_3.
   C_1 is the C note written on the first ledger line below the treble clef. That is the C in the
   middle of a piano keyboard,  where the commercial sign usualle appears   }
 C_0 = 65;                 C = 131;               C_1 = 261;                  C_2 = 523;                  C_3 = 1046;
             Cp_0 = 69;                Cp = 138;                Cp_1 = 277;                 Cp_2 = 554;                 Cp_3 = 1109;
             //Db_0 = Cp_0;              Db = Cp;                 Db_1 = Cp_1;                Db_2 = Cp_2;                Db_3 = Cp_3;
 D_0 = 73;                 D = 149;               D_1 = 293;                  D_2 = 587;                  D_3 = 1174;
             Dp_0 =78 ;                Dp = 155;                Dp_1 = 311;                 Dp_2 = 622;                 Dp_3 = 1244;
             //Eb_0 = Dp_0;              Eb = Dp;                 Eb_1 = Dp_1;                Eb_2 = Dp_2;                Eb_3 = Dp_3;
 E_0 = 82;                 E = 165;               E_1 = 329;                  E_2 = 659;                  E_3 = 1318;
 F_0 = 87;                 F = 174;               F_1 = 349;                  F_2 = 698;                  F_3 = 1397;
             Fp_0 = 92;                Fp = 189;                Fp_1 = 370;                 Fp_2 = 740;                 Fp_3 = 1480;
             //Gb_0 = Fp_0;              Gb = Fp;                 Gb_1 = Fp_1;                Gb_2 = Fp_2;                Gb_3 = Fp_3;
 G_0 = 98;                 G = 196;               G_1 = 392;                  G_2 = 784;                  G_3 = 1568;
             Gp_0 = 104;               Gp = 207;                Gp_1 = 415;                 Gp_2 = 830;                 Gp_3 = 1661;
             //Ab_0 = Fp_0;              Ab = Gp;                 Ab_1 = Gp_1;                Ab_2 = Gp_2;                Ab_3 = Gp_3;
 A_0 = 110;                A = 220;               A_1 = 440;                  A_2 = 880;                  A_3 = 1760;
             Ap_0 = 116;               Ap = 233;                Ap_1 = 466;                 Ap_2 = 932;                 Ap_3 = 1864;
             //Bb_0 = Ap_0;              Bb = Ap;                 Bb_1 = Ap_1;                Bb_2 = Ap_2;                Bb_3 = Ap_3;
 B_0 = 123;                B = 247;               B_1 = 494;                  B_2 = 988;                  B_3 = 1975;


                                     //
 aFreqs : array[0..60] of integer  =  (  65,    69,    73,     78,    82,     87,    92,    98,    104,   110,   116,    123,
                                        131,   138,   149,    155,   165,    174,   189,   196,    207,   220,   233,    247,
                                        261,   277,   293,    311,   329,    349,   370,   392,    415,   440,   466,    494,
                                        523,   554,   587,    622,   659,    698,   740,   784,    830,   880,   932,    988,
                                       1046,  1109,  1174,   1244,  1318,   1397,  1480,   1568,   1661,  1760,  1864,  1975,
                                          1);

Tones : array[0..60] of string[4] = ( 'C_0', 'Cp_0', 'D_0', 'Dp_0', 'E_0', 'F_0', 'Fp_0', 'G_0', 'Gp_0', 'A_0', 'Ap_0', 'B_0',
                                        'C',   'Cp',   'D',   'Dp',   'E',   'F',   'Fp',   'G',   'Gp',   'A',   'Ap',   'B',
                                      'C_1', 'Cp_1', 'D_1', 'Dp_1', 'E_1', 'F_1', 'Fp_1', 'G_1', 'Gp_1', 'A_1', 'Ap_1', 'B_1',
                                      'C_2', 'Cp_2', 'D_2', 'Dp_2', 'E_2', 'F_2', 'Fp_2', 'G_2', 'Gp_2', 'A_2', 'Ap_2', 'B_2',
                                      'C_3', 'Cp_3', 'D_3', 'Dp_3', 'E_3', 'F_3', 'Fp_3', 'G_3', 'Gp_3', 'A_3', 'Ap_3', 'B_3',
                                      'REST');

THE_END  = 'FINE';





type
  TPresetSound = ( psOK,
                   psError,
                   psWelcome,
                   psEmergency,
                   psWrong,
                   psCall,
                   psOfficial,
                   psDaze,
                   psFall,
                   psChord,
                   psWhisle,
                   psHanging,
                   psClimb );


  TBeatDuration = (bd_500, bd_1000, bd_1500, bd_2000);


type
(*--------------------------------------------------------------------------------*)
  TbtBeeper = class(TComponent)
  private
    FBeatDuration  : TBeatDuration;
    FDuration      : integer;
    FDefaultSound  : TPresetSound;
    SoundList      : TObject;
    SoundThread    : TThread;
    procedure SetBeatDuration(Value:TBeatDuration);
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure PlayDefaultSound;
    procedure PlayPresetSound(Sound:TPresetSound);
    procedure BeepSequence(ToneAndMSecs: array of DWord);
    procedure BeepFor(Tone : Word; MSecs : DWORD);
    procedure Beep(Tone : Word);
    procedure Pause;
    procedure PauseFor(MSecs : DWORD);
    procedure PlayTextFile(FileName, Song: string);
  published
    { BeatDuration affects only the Beep and the Pause methods}
    property BeatDuration : TBeatDuration read FBeatDuration write SetBeatDuration default bd_1000;
    property DefaultSound : TPresetSound  read FDefaultSound write FDefaultSound default psOK;
  end;


procedure Register;

procedure StartBeep(Freq : Word);
procedure StopBeep;


implementation

{$R btOdeum.res}       

type
  EBeepError = class(Exception);


(*----------------------------------------------------------------------------------*)
procedure RaiseError(const Msg: string);
begin
  raise EBeepError.Create(Msg);
end;
(*----------------------------------------------------------------------------------*)
procedure RaiseErrorFmt(const Msg: string; const Args: array of const);
begin
  raise EBeepError.CreateFmt(Msg, Args);
end;




{ utilities }
(*--------------------------------------------------------------------------------*)
procedure Register;
begin
   RegisterComponents('btControls', [TbtBeeper]);
end;
{John Atkins [jatkins@paktel.compulink.co.uk] functions for playing beeps}
(*--------------------------------------------------------------------------------*)
function _GetPort(address:word):word;
var
 bValue: byte;
begin
  asm
    mov dx, address
    in al, dx
    mov bValue, al
  end;
  Result := bValue;
end;
(*--------------------------------------------------------------------------------*)
procedure _SetPort(address, Value:Word);
var
 bValue: byte;
begin
  bValue := Trunc(Value and 255);
  asm
    mov dx, address
    mov al, bValue
    out dx, al
  end;
end;
(*--------------------------------------------------------------------------------*)
procedure StartBeep(Freq : Word);
var
  B: Byte;
begin
  if (Freq >= LOW_FREQ) and (Freq <= HIGH_FREQ) then
  begin
    Freq := Word(1193181 div LongInt(Freq));
    B := Byte(_GetPort($61));
    if (B and 3) = 0 then
    begin
      _SetPort($61, Word(B or 3));
      _SetPort($43, $B6);
    end;
    _SetPort($42, Freq);
    _SetPort($42, Freq shr 8);
  end;
end;
(*--------------------------------------------------------------------------------*)
procedure StopBeep;
var
 Value: Word;
begin
  Value := _GetPort($61) and $FC;
  _SetPort($61, Value);
end;  








{ TCmd }
type
(*--------------------------------------------------------------------------------*)
  TCmd = class
  public
    Tone     : Word;
    Duration : DWORD;
    procedure DoSound;
    constructor Create(Tone: Word; Duration: DWORD);      
  end;   

(*--------------------------------------------------------------------------------*)
constructor TCmd.Create(Tone: Word; Duration: DWORD);
begin
  inherited Create;
  Self.Tone := Tone;
  Self.Duration := Duration;
end;
(*--------------------------------------------------------------------------------
 Description : generates a Tone a MSecs long
 Notes       : ElapsedMillisecFrom and the waiting loop is replaced by
               simple calls to Sleep() as suggested by Peter Williams [pew@pcug.org.au]
---------------------------------------------------------------------------------*)
procedure TCmd.DoSound;
begin

  if Tone = REST then
  begin
    Sleep(Duration);
    Exit;
  end;

  if (Win32Platform = VER_PLATFORM_WIN32_NT) then
     Windows.Beep (Tone, Duration)
  else begin
    StartBeep(Tone);
    Sleep(Duration);
    StopBeep;  
  end;  

end;




{ TCriticalSection }
type     
(*--------------------------------------------------------------------------------*)
  TCriticalSection = class (TObject)
  private
    FCS: TRTLCriticalSection;
  public
    constructor Create; 
    destructor Destroy; override;
    procedure Enter;
    procedure Leave;
  end;

(*--------------------------------------------------------------------------------*)
constructor TCriticalSection.Create;
begin
  inherited Create;
  InitializeCriticalSection(FCS);
end;
(*--------------------------------------------------------------------------------*)
destructor TCriticalSection.Destroy;
begin
  DeleteCriticalSection(FCS);
  inherited Destroy;
end;
(*--------------------------------------------------------------------------------*)
procedure TCriticalSection.Enter;
begin
  EnterCriticalSection(FCS);
end;
(*--------------------------------------------------------------------------------*)
procedure TCriticalSection.Leave;
begin
  LeaveCriticalSection(FCS);
end;





{ TProtected - TQueueList}
type  
(*--------------------------------------------------------------------------------*)
  TProtected = class
  protected
    CS   : TCriticalSection;
    List : TStringList;
  public
    constructor Create;
    destructor Destroy; override;
  end;

  TQueueAction = (qaPushTop, qaPushBottom, qaPop);       
(*--------------------------------------------------------------------------------*)
  TQueueList = class(TProtected)
  private
    Thread : TThread;
    procedure AccessItem(var Obj: TObject; Action: TQueueAction);  // TQueueAction = (qaPushTop, qaPushBottom, qaPop);
  public
    constructor Create(T: TThread);
    destructor Destroy; override;

    procedure Push(Obj: TObject);
    procedure PushTop(Obj: TObject);
    function  Pop: TObject;
  end;



{ TProtected }
(*--------------------------------------------------------------------------------*)
constructor TProtected.Create;
begin
  inherited Create;
  CS := TCriticalSection.Create;
  List := TStringList.Create;
end;
(*--------------------------------------------------------------------------------*)
destructor TProtected.Destroy;
begin
  CS.Free;
  List.Free;
  inherited Destroy;
end;



{ TQueueList }
(*--------------------------------------------------------------------------------*)
constructor TQueueList.Create(T: TThread);
begin
  inherited Create;
  Thread := T;
end;
(*--------------------------------------------------------------------------------*)
destructor TQueueList.Destroy;
begin
  while List.Count > 0 do
  begin
    TCmd(List.Objects[0]).Free;
    List.Delete(0);
  end;
  inherited Destroy;
end;
(*--------------------------------------------------------------------------------*)
procedure TQueueList.AccessItem(var Obj: TObject; Action: TQueueAction);
begin
  CS.Enter;
  try
    case Action of
      qaPushTop    : begin
                       if List.Count = 0 then
                         List.AddObject('', Obj)
                       else List.InsertObject(0, '', Obj);

                       if Thread <> nil then
                         if Thread.Suspended then
                           Thread.Resume;  { resume the Thread if needed }
                     end;
      qaPushBottom : begin
                       List.AddObject('', Obj);
                       if Thread <> nil then
                         if Thread.Suspended then
                           Thread.Resume;  { resume the Thread if needed }
                     end;
      qaPop        : begin
                       if List.Count = 0 then
                       begin
                         Obj := nil;
                         Exit;
                       end;
                       Obj := TObject(List.Objects[0]);
                       List.Delete(0);
                     end;
    end;
  finally
    CS.Leave;
  end;
end;
(*--------------------------------------------------------------------------------*)
function TQueueList.Pop: TObject;
begin
  AccessItem(Result, qaPop);
end;
(*--------------------------------------------------------------------------------*)
procedure TQueueList.Push(Obj: TObject);
begin
  AccessItem(Obj, qaPushBottom);
end;
(*--------------------------------------------------------------------------------*)
procedure TQueueList.PushTop(Obj: TObject);
begin
  AccessItem(Obj, qaPushTop);
end;













{ TSoundThread }
type
(*--------------------------------------------------------------------------------*)
  TSoundThread = class(TThread)
  private
    Beeper : TbtBeeper;
  protected
    procedure Execute; override;
  public
    constructor Create(Beeper : TbtBeeper);
    procedure ResumeThread;
    procedure SuspendThread;
  end;



(*--------------------------------------------------------------------------------*)
constructor TSoundThread.Create(Beeper : TbtBeeper);
begin
  inherited Create(True);       { Create thread suspended }
  Priority        := tpNormal;
  FreeOnTerminate := False;     { Thread does not free Itself when terminated }
  Self.Beeper     := Beeper;
end;
(*--------------------------------------------------------------------------------*)
procedure TSoundThread.ResumeThread;
begin
 if Suspended then Resume;
end;
(*--------------------------------------------------------------------------------*)
procedure TSoundThread.SuspendThread;
begin
  if not Suspended then Suspend;
end;
(*--------------------------------------------------------------------------------*)
procedure TSoundThread.Execute;
var
  Cmd : TCmd;
begin   
  while (Terminated = False) do
  begin
    Cmd := TCmd(TQueueList(Beeper.SoundList).Pop);  
    if (Cmd = nil) then
      SuspendThread
    else begin
      Cmd.DoSound;
      Cmd.Free;
    end;
  end;
end;






{ TbtBeeper }
(*--------------------------------------------------------------------------------*)
constructor TbtBeeper.Create(AOwner:TComponent);
begin
  inherited Create(AOwner);
  FBeatDuration  := bd_1000;
  FDuration      := 1000;
  FDefaultSound  := psOK;
  SoundThread    := TSoundThread.Create(Self);
  SoundList      := TQueueList.Create(SoundThread);
end;
(*--------------------------------------------------------------------------------*)
destructor TbtBeeper.Destroy;
begin
  SoundThread.Terminate;
  repeat
    Application.ProcessMessages;
  until TSoundThread(SoundThread).Terminated;
    
  SoundThread.Free;
  SoundList.Free;

  inherited Destroy;
end;
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.PlayDefaultSound;
begin
  PlayPresetSound(FDefaultSound);
end;
(*--------------------------------------------------------------------------------
 Description  : plays a sequence of sounds
 Example call : BeepSequence([F_2, 200,
                              B_1, 200,
                              F_2, 200 ]);
 Author       : Friedrich Ing. Hajny [Cum_Hajny@compuserve.com]
---------------------------------------------------------------------------------*)
procedure TbtBeeper.BeepSequence(ToneAndMSecs: array of DWord);
var
  Inx: Integer;
begin
  if Length(ToneAndMSecs) = 0 then Exit;
  for Inx := Low(ToneAndMSecs) to (Pred(High(ToneAndMSecs)) div 2) do   
    TQueueList(SoundList).Push(TCmd.Create(ToneAndMSecs[Inx * 2], ToneAndMSecs[Succ(Inx * 2)]));
end;
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.PlayPresetSound(Sound:TPresetSound);
begin     

  case Sound of
    psOK          : begin
                      BeepFor (Ap_2,100);
                      BeepFor (B_2, 100);
                      BeepFor (C_3, 100);
                    end;   
    psError       : begin
                      BeepFor (Fp_0,150);
                      BeepFor (REST,200);
                      BeepFor (C_0,500);
                    end; 
    psWelcome     : begin
                      BeepFor (Ap_2,100);
                      BeepFor (B_2, 100);
                      BeepFor (C_3, 100);
                      BeepFor (REST,100);
                      BeepFor (C_3, 100);
                      BeepFor (B_2, 100);
                      BeepFor (Ap_2,100);
                    end;   
    psEmergency   : begin
                      BeepFor (F_2,200);
                      BeepFor (B_1, 200);
                      BeepFor (F_2,200);
                      BeepFor (B_1, 200);
                      BeepFor (F_2,200);
                      BeepFor (B_1, 200);
                      BeepFor (F_2,200);
                      BeepFor (B_1, 200);
                    end; 
    psWrong       : begin
                      BeepFor (C_1,150);
                      BeepFor (B,50);
                      BeepFor (Ap,50);
                      BeepFor (A,50);
                      BeepFor (Gp,50);
                      BeepFor (G,50);
                      BeepFor (Fp,50);
                      BeepFor (F,50);
                      BeepFor (E,50);
                      BeepFor (Dp,50);
                      BeepFor (D,50);
                      BeepFor (Cp,50);
                      BeepFor (C,100);
                      BeepFor (C_0,200);
                    end;
    psCall        : begin
                      BeepFor (G,650);
                      BeepFor (REST,100);
                      BeepFor (E,500);
                    end;
    psOfficial    : begin
                      BeepFor (G,200);
                      BeepFor (REST,50);
                      BeepFor (G,200);
                      BeepFor (REST,50);
                      BeepFor (G,200);
                      BeepFor (REST,50);
                      BeepFor (E,700);
                      BeepFor (REST,100);
                      BeepFor (C_1,200);
                      BeepFor (REST,50);
                      BeepFor (C_1,200);
                      BeepFor (REST,50);
                      BeepFor (C_1,200);
                      BeepFor (REST,50);
                      BeepFor (C,700);
                    end;  
    psDaze        : begin
                      BeepFor (E_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (D_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (E_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (D_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (E_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (D_1,100);
                      BeepFor (Dp_1,100);
                    end;
    psFall        : begin
                      BeepFor (E_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (D_1,100);
                      BeepFor (Dp_1,100);

                      BeepFor (F_1,100);
                      BeepFor (E_1,100);
                      BeepFor (Dp_1,100);
                      BeepFor (E_1,100);

                      BeepFor (Fp_1,100);
                      BeepFor (F_1,100);
                      BeepFor (E_1,100);
                      BeepFor (F_1,100);
                    end;
    psChord       : begin
                      BeepFor (B_1,80);
                      BeepFor (Ap_1,80);
                      BeepFor (A_1,80);
                      BeepFor (Gp_1,80);
                      BeepFor (G_1,80);
                      BeepFor (Fp_1,80);
                      BeepFor (F_1,80);
                      BeepFor (E_1,80);
                      BeepFor (Dp_1,80);
                      BeepFor (D_1,80);
                      BeepFor (Cp_1,80);
                      BeepFor (C_1,80);
                    end;
    psWhisle      : begin
                      BeepFor (C_2,80);
                      BeepFor (F_2,80);
                      BeepFor (G_2,80);
                      BeepFor (C_3,80);
                    end;  
    psHanging     : begin
                      BeepFor (G_2,80);
                      BeepFor (C_3,80);
                      BeepFor (Gp_2,80);
                      BeepFor (Cp_3,80);
                      BeepFor (A_2,80);
                      BeepFor (D_3,80);
                      BeepFor (Ap_2,80);
                      BeepFor (Dp_3,80);
                      BeepFor (B_2,80);
                      BeepFor (E_3,80);
                      BeepFor (C_3,80);
                      BeepFor (F_3,80);
                    end;
    psClimb       : begin
                      BeepFor (C_1,80);
                      BeepFor (Cp_1,80);
                      BeepFor (D_1,80);
                      BeepFor (Dp_1,80);
                      BeepFor (E_1,80);
                      BeepFor (F_1,80);
                      BeepFor (Fp_1,80);
                      BeepFor (G_1,80);
                      BeepFor (Gp_1,80);
                      BeepFor (A_1,80);
                      BeepFor (Ap_1,80);
                      BeepFor (B_1,80);
                    end;
  end;

end;
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.Beep(Tone : word);
begin
  BeepFor(Tone, FDuration);
end;              
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.BeepFor(Tone : Word; MSecs : DWORD);
begin
  TQueueList(SoundList).Push(TCmd.Create(Tone, MSecs));
end;  
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.Pause;
begin
  PauseFor(FDuration);
end;     
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.PauseFor(MSecs : DWORD);
begin
  BeepFor(REST, MSecs);
end;
(*--------------------------------------------------------------------------------*)
procedure TbtBeeper.SetBeatDuration(Value:TBeatDuration);
begin
  if Value <> FBeatDuration then           
  begin
    FBeatDuration := Value;
    case Value of
      bd_500  : FDuration := 500 ;
      bd_1000 : FDuration := 1000;
      bd_1500 : FDuration := 1500;
      bd_2000 : FDuration := 2000;
    end;
  end;
end;
(*--------------------------------------------------------------------------------
 Description : opens an ascii file and plays a song
               This file can be written with any text editor like notepad.
               The form of the file:
               <song title>
               <freq const>, <duration>,
               <freq const>, <duration>,
               <freq const>, <duration>,
               .
               .
               .
               FINE
 Note        : see Songs.txt for an example
               You can have more than one song in the same file
---------------------------------------------------------------------------------*)
procedure TbtBeeper.PlayTextFile(FileName, Song: string);
const
  InValidChars = [#0..#47,#58..#64, #91..#94, #96, #123..#255];
var
 Stream            : TMemoryStream;
 szFirst,
 szLast,
 szHolder          : PChar;
 sTone,
 sMSecs            : string;
 i                 : integer;
begin


  Stream:=TMemoryStream.Create;
  try                          
    Stream.LoadFromFile(FileName);

    sTone  :='';
    sMSecs :='';

    szFirst:= StrPos( Stream.Memory, PChar(Song) );

    if szFirst = nil  then
    begin
      RaiseErrorFmt('Can Not Locate Song %s in %s', [Song, FileName]);
    end
    else begin
      GetMem(szHolder, 5);                                       // get mem for the holder PChar
      try
        Inc(szFirst, Length(Song) + 1);                          // move szFirst after song title
        while True  do                                           // loop for ever
        begin
          while szFirst^ in InValidChars do Inc(szFirst);        // skip blanks
          FillChar(szHolder^, 5, #0);                            // zero the szHolder
          StrLCopy(szHolder, szFirst, 4);                        // get first 4 chars
          if String(szHolder) = THE_END then Break;

          szLast:= StrScan( szFirst, ',' );                      // look for the next comma
          if szLast = nil then                                   // if ok
           RaiseError('Beeper: Wrong Char');

          FillChar(szHolder^, 5, #0);                            // zero the szHolder
          StrLCopy(szHolder, szFirst, (szLast ) - szFirst);      // copy chars until szLast - 1 to szHolder
          sTone:=StrPas(szHolder);                               // convert it to Pascal string
          szFirst:=szLast + 1;                                   // move szFirst after next comma

          while szFirst^ in InValidChars do Inc(szFirst);        // skip blanks
          szLast:= StrScan( szFirst, ',' );                      // look for the next comma
          if szLast = nil then                                   // if ok
           RaiseError('Beeper: Wrong Char');

          FillChar(szHolder^, 5, #0);                            // zero the szHolder
          StrLCopy(szHolder, szFirst, (szLast) - szFirst);       // copy chars until szLast - 1 to szHolder
          sMSecs:=StrPas(szHolder);                              // convert it to Pascal string
          szFirst:=szLast + 1;                                   // move szFirst after next comma

          for i:= 0 to 60 do
          if sTone = Tones[i] then                               // play the sound
          begin
            BeepFor(aFreqs[i], StrToInt(sMSecs));
            Break;
          end;
        end;
      finally
        FreeMem(szHolder, 5);
      end;
    end;

  finally
    Stream.Clear;
    Stream.Free;
  end;
end; 






















end.













