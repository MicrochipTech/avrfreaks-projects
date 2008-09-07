unit Main;

interface

{******************************************************************************}
{**                                                                          **}
{**  MyUSBHID, demo client program for vendor defined HID demo using the     **}
{**     MyUSB library.                                                       **}
{**                                                                          **}
{**  March 2008, P.Harvey-Smith, V1.0                                        **}
{**                                                                          **}
{**  This program communicates with a USB enabled AVR and allows the PC to   **}
{**  1) Read and write port values : PORTx, PINx and DDRx, this allows the   **}
{**  PC to remotely control the AVR's I/O lines.                             **}
{**                                                                          **}
{**  2) Read and write bytes in EEPROM.                                      **}
{**                                                                          **}
{**  3) Read and write bytes in RAM ***DANGER*** this can crash the AVR if   **}
{**  you overwrite something essential, like the stack !                     **}
{**                                                                          **}
{**                                                                          **}
{**  MyUSBHID was developed using Borland Delphi 7, but should compile on a  **}
{**  later version with very little problem, it may also compile on Delphi 5 **}
{**  or 6 (all of these are untested !). To compile the code you will need to**}
{**  have the JediVCL installed, as it uses the HID component from Jedi.     **}
{**  The Jedi VCL is available for free from : http://www.delphi-jedi.org/   **}
{**                                                                          **}
{**  Note, both this and the AVR firmware could be made more streamlined and **}
{**  efficient at the cost of readability, since this is intended to be a    **}
{**  demonstration of how to use custom HID reports, I tried to keep things  **}
{**  as readable as possible !                                               **}
{**                                                                          **}
{**  2008-03-20, PHS,                                                        **}
{**  Getting the sizes of the EEPROM and RAM now uses a feature report, this **}
{**  simplifies things both at the host and the device end as a request to   **}
{**  access EEPROM/RAM can now only be a read or a write.                    **}
{**                                                                          **}
{**  If you have any questions of comments about this code or the            **}
{**  corresponding AVR fromware please feel free to contactm me either via   **}
{**  the forum on AVRFreaks (http://www.avrfreaks.net) user Prime. Or via    **}
{**  email at : afra AT aurigae DOT demon DOT co DOT uk                      **}
{**                                                                          **}
{******************************************************************************}

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, JvComponentBase, JvHidControllerClass, StdCtrls, ComCtrls;

CONST
	{Name of the HID device, used to recognise it when enumerating}
	MyUSB	= 'MyUSB AVRPort Demo';

	{ Defined HID port IDs, these are defined in Descriptors.h in the }
        { AVR firmware source, and *MUST* have the same values.           }
        { Note these all currently have the same value, however they may  }
        { in future not have, so I have used symbolic constants !         }
	HIDReportPortIn		= $01;	// Report of Port/EEProm Read
        HIDReportPortOut        = $01;	// Report for Port write
        HIDReportEEPROMOut	= $01;	// Report for EEPROM/RAM write

	{Valid report types known to us }
        ValidReports	= [HIDReportPortIn,
        		   HIDReportPortOut,
        		   HIDReportEEPROMOut];

        {Valid report for input}
        ValidReportsIn	= [HIDReportPortIn];

        {Valid reports for output}
        ValidReportsOut	= [HIDReportPortOut,
        		   HIDReportEEPROMOut];

	{AVR Port defines}
	APortA	= $00;
        APortB	= $01;
        APortC	= $02;
        APortD	= $03;
        APortE	= $04;
        APortF	= $05;
        APortG	= $06;

        APIN	= $00;
        APDDR	= $10;
        APOUT	= $20;

        {Port defines compatible with AVR asm/GCC}
        PinA	= APortA+APIN;
        DDRA	= APortA+APDDR;
        PortA	= APortA+APOUT;

        PinB	= APortB+APIN;
        DDRB	= APortB+APDDR;
        PortB	= APortB+APOUT;

        PinC	= APortC+APIN;
        DDRC	= APortC+APDDR;
        PortC	= APortC+APOUT;

        PinD	= APortD+APIN;
        DDRD	= APortD+APDDR;
        PortD	= APortD+APOUT;

        PinE	= APortE+APIN;
        DDRE	= APortE+APDDR;
        PortE	= APortE+APOUT;

        PinF	= APortF+APIN;
        DDRF	= APortF+APDDR;
        PortF	= APortF+APOUT;

        PWrite	= $40;

        {Destination byte values, these determine how to interpret the command byte}
        DestPort	= $00;		// Port I/O
        DestEEPROM	= $01;		// EEPROM read/write
        DestRAM		= $02;		// RAM read/write

	{Masks within command byte, when Dest=Port}
        PortMask	= $0F;
        RegMask		= $30;
        WriteMask	= $40;
        ErrorMask	= $80;

        {Shift to turn reg offet into command byte, or vice-versa}
        RegShift	= 4;

        {Command byte when Dest=EEPROM or RAM}
	OPMask		= $3F;		// Operation mask, to extract op code from command byte
	ReadOrWrite	= $00;		// Read or write value, W in CmdByte determines r or w
	GetAddressMax	= $01;		// Get Max value for address

        Signon		= 'MyUSB-HID, V1.0, 2008-03-21, P.Harvey-Smith';
type

  {AVR Port record, used to hold current values of ports, and port valid flag}
  TAVRPort = PACKED RECORD
  	PortValid	: BOOLEAN;	{ If this port is valid on the current AVR}
        PortDDR		: BYTE;		{ Current values for DDR, PIN and PORT}
        PortIN		: BYTE;
        PortOUT		: BYTE;
  END;

  {Record to hold the report sent to / received from the AVR over USB}
  TAVRIORecordPort	= PACKED RECORD
  	CmdByte		: BYTE; 	{ Command byte }
        DestByte	: BYTE;		{ Destination Byte }
        DataByte        : BYTE;		{ Data byte }
  END;

  {Record to hold data read/written from memory, EEPROM or RAM}
  TAVRIORecordEE_RAM	= PACKED RECORD
  	CmdByte		: BYTE; 	{ Command byte }
        DestByte	: BYTE;		{ Destination byte }
        DataByte        : BYTE;		{ Data byte }
	Address		: WORD;		{ EEPROM Address }
  END;

  {AVRIO Record, either a Port IO or a memory access}
  TAVRIORecord	= PACKED RECORD
  CASE INTEGER OF
  	0  : (Port 		: TAVRIORecordPort);
        1  : (EEPROM_RAM	: TAVRIORecordEE_RAM);
  END;

  {Pointer type to above record, used for reading from the HID}
  TPAVRIORecord = ^TAVRIORecord;

  {AVR report used to pass to IO procedure}
  TAVRReport	= PACKED RECORD
  	ReportNo	: BYTE;
        IORecord	: TAVRIORecord;
  END;

  {Feature request packet, used to get EEPROM & RAM size}
  TFeatureIO	= PACKED RECORD
  	ReportID	: BYTE;
        CmdByte		: BYTE;
        EESize		: WORD;
        RamSize		: WORD;
  END;

  THIDForm = class(TForm)
    HidCtrl: TJvHidDeviceController;
    AvrMemo: TMemo;
    PortCombo: TComboBox;
    Port: TLabel;
    Label1: TLabel;
    RegCombo: TComboBox;
    GroupBox1: TGroupBox;
    Value: TEdit;
    Label2: TLabel;
    ReadPortBtn: TButton;
    WritePortBtn: TButton;
    GroupBox2: TGroupBox;
    Label3: TLabel;
    EEAddress: TEdit;
    Label4: TLabel;
    EEValue: TEdit;
    EERead: TButton;
    EEWrite: TButton;
    GroupBox3: TGroupBox;
    Label5: TLabel;
    Label6: TLabel;
    RAMAddress: TEdit;
    RAMValue: TEdit;
    RAMRead: TButton;
    RAMWrite: TButton;
    GroupBox4: TGroupBox;
    GetFeatureBtn: TButton;
    SetFeatureBtn: TButton;
    Button1: TButton;
    procedure HidCtrlArrival(HidDev: TJvHidDevice);
    procedure FormCreate(Sender: TObject);
    procedure HidCtrlRemoval(HidDev: TJvHidDevice);
    procedure HidCtrlDeviceChange(Sender: TObject);
    procedure ReadPortBtnClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure WritePortBtnClick(Sender: TObject);
    procedure EEReadClick(Sender: TObject);
    procedure EEWriteClick(Sender: TObject);
    procedure RAMReadClick(Sender: TObject);
    procedure RAMWriteClick(Sender: TObject);
    procedure GetFeatureBtnClick(Sender: TObject);
    procedure SetFeatureBtnClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
    AVRPorts	: ARRAY[0..6]OF TAVRPort;	{ Array holding current port values }
    AVR		: TJvHidDevice;			{ The HID device for communicating with the AVR}

    Replied	: BOOLEAN;			{Has the AVR replied yet ?}
    Reply	: TAVRReport;			{Reply packet from AVR}

    Initialized	: BOOLEAN;

    MaxEEPROM	: INTEGER;
    MaxRAM	: INTEGER;

    PROCEDURE Msg(ToShow	: STRING);
    PROCEDURE MsgFormat(ToShow	: STRING;
    			Params	: ARRAY OF CONST);

    PROCEDURE FreeAVR;
    PROCEDURE AssignAVR(Dev	: TJvHidDevice);
    PROCEDURE AVRIO(VAR AVRReport	: TAVRReport); overload;
    PROCEDURE AVRIO(VAR AVRReport	: TAVRReport;
    		    ReportID		: BYTE); overload;
    PROCEDURE VerboseAVRIO(VAR AVRReport	: TAVRReport);

    FUNCTION DoAVRIO(Port	: BYTE;
    		     Value	: BYTE;
    		     IsWrite	: BOOLEAN) : BYTE;

    PROCEDURE AVRReply(HidDev	: TJvHidDevice;
    		       ReportID	: BYTE;
  		 CONST Data	: Pointer;
                       Size	: Word);
    PROCEDURE AVRError(HidDev: TJvHidDevice; Error: DWORD);
    PROCEDURE GETAVRPortValues(PortNo		: BYTE;
			       VAR PortIO	: TAVRPort);
    PROCEDURE InitPorts;
    PROCEDURE InitMem;
    PROCEDURE SetupPortIO(VAR AVRIORecord	: TAVRReport;
    		      	  IsWrite		: BOOLEAN);
    PROCEDURE SetupControls(State	: BOOLEAN);
    PROCEDURE SetupEE_RAM_IO(VAR AVRIORecord	: TAVRReport;
    		      	     IsWrite		: BOOLEAN;
                             Dest		: BYTE);
    PROCEDURE EE_RAM_ReadWrite(IsWrite	: BOOLEAN;
    			       Dest	: BYTE);
  public
    { Public declarations }
  end;

var
  HIDForm: THIDForm;

implementation

{$R *.dfm}

{Free the AVR HID record}
PROCEDURE THIDForm.FreeAVR;

BEGIN;
  {First check to see if it is nil, if not, release the recieve handler}
  IF (AVR<>NIL) THEN
  BEGIN;
    AVR.OnData:=NIL;
    AVR.OnDataError:=NIL;
  END;

  {Flag no AVR communication by setting to NIL}
  AVR:=NIL;

  {Disable AVR controls}
  SetupControls(FALSE);
END;

{Assign the HID device to the AVR hid record so that we can communicate}
PROCEDURE THIDForm.AssignAVR(Dev	: TJvHidDevice);

BEGIN;
  FreeAVR;			// Free any existing record
  AVR:=Dev;                    	// Assign new device
  AVR.OnData:=AVRReply;	      	// Assign function to handle incoming data
  AVR.OnDataError:=AVRError;	// Assign function to handle errors

  WITH AVR.Caps DO
  BEGIN;
    MsgFormat('Length of input report=%d',[InputReportByteLength]);
    MsgFormat('Length of output report=%d',[OutputReportByteLength]);
    MsgFormat('Length of feature report=%d',[FeatureReportByteLength]);
  END;

  InitPorts;			// Poll AVR over USB link, to determine what ports are valid
  InitMem;			// Get max values for EEPROM and RAM
  SetupControls(TRUE);
END;

{Perform an IO operation to/from the AVR over the USB link, note this always}
{writes the size of the maximum report size +1 (for report id)}
PROCEDURE THIDForm.AVRIO(VAR AVRReport	: TAVRReport);

VAR
  	BytesWritten		: Cardinal;	// Number of bytes sent

BEGIN;
  {Check that we have a valid USB link}
  IF ((Assigned(AVR)) AND (AVRReport.ReportNo IN ValidReportsOut)) THEN
  BEGIN;
    Replied:=FALSE;			// Flag no reply
    AVR.WriteFile(AVRReport,SizeOF(AVRReport),BytesWritten);

    WHILE (NOT Replied) DO 		// Wait for a reply, fixme, no timeout, waits forever !
      Application.ProcessMessages;

    AVRReport:=Reply;			// Send the reply to the caller
  END;
END;

PROCEDURE THIDForm.AVRIO(VAR AVRReport	: TAVRReport;
    		         ReportID	: BYTE);

BEGIN;
  AVRReport.ReportNo:=ReportID;
  AVRIO(AVRReport);
END;

PROCEDURE THIDForm.VerboseAVRIO(VAR AVRReport	: TAVRReport);

VAR	PortName	: STRING;
	RegName		: STRING;
        Direction	: STRING;
    	Address		: STRING;
        Data		: STRING;
   	OutLine		: STRING;

BEGIN;
  IF ((AVRReport.IORecord.Port.CmdByte AND WriteMask)=WriteMask) THEN
    Direction:='Write'
  ELSE
    Direction:='Read';

  Data:=Format('%2.2X',[AVRReport.IORecord.Port.DataByte]);

  IF (AVRReport.IORecord.Port.DestByte IN [DestEEPROM,DestRAM]) THEN
    Address:=Format('%4.4X',[AVRReport.IORecord.EEPROM_RAM.Address])
  ELSE
    Address:='';

  CASE (AVRReport.IORecord.Port.DestByte) OF
    DestPort	: BEGIN;
    		    PortName:=CHR((AVRReport.IORecord.Port.CmdByte AND PortMask)+65);
                    CASE (AVRReport.IORecord.Port.CmdByte AND RegMask) OF
                      APIN	: RegName:='PIN'+PortName+'  ';
                      APDDR	: RegName:='DDR'+PortName+'  ';
                      APOUT	: RegName:='PORT'+PortName+' ';
                    END;
                    OutLine:=Format('%s %s data=%s',[RegName,Direction,Data]);
    		  END;
    DestEEPROM	: OutLine:=Format('EEPROM %s at address %s data=%s',[Direction,Address,Data]);
    DestRAM	: OutLine:=Format('RAM %s at address %s data=%s',[Direction,Address,Data]);
  END;

  AVRIO(AVRReport);

  IF ((AVRReport.IORecord.Port.CmdByte AND ErrorMask)=ErrorMask) THEN
    MsgFormat('%s failed ! error code=%2.2X',[OutLine,AVRReport.IORecord.Port.DataByte])
  ELSE
    MsgFormat('%s suceeded ! data byte=%2.2X',[OutLine,AVRReport.IORecord.Port.DataByte])
END;

FUNCTION THIDForm.DoAVRIO(Port		: BYTE;
    		          Value		: BYTE;
    		          IsWrite	: BOOLEAN) : BYTE;

VAR 	AVRReport	: TAVRReport;

BEGIN;
  WITH AVRReport DO
  BEGIN;
    IF IsWrite THEN
    BEGIN;
      ReportNo:=HIDReportPortOut;
      IORecord.Port.CmdByte:=Port+WriteMask;
      IORecord.Port.DataByte:=Value;
    END
    ELSE
    BEGIN;
      ReportNo:=HIDReportPortIn;
      IORecord.Port.CmdByte:=Port;
    END;
    IORecord.Port.DestByte:=DestPort;
  END;
  VerboseAVRIO(AVRReport);

  Result:=AVRReport.IORecord.Port.DataByte;
END;


{Handler event that recieves data from the USB channel}
PROCEDURE THIDForm.AVRReply(HidDev	: TJvHidDevice;	// Device sending data
    		       	    ReportID	: BYTE;		// Report ID
  		      CONST Data	: Pointer;	// Data from HID device
                            Size	: Word);	// Size of data from HID device

VAR	IORecord	: TPAVRIORecord;	// Pointer to AVR data, used to easily typecast !

BEGIN;
  // Fixme, we should check here that this is the correct sender.
  IF (ReportID IN ValidReports) THEN		// check for valid report ID
  BEGIN;
    IORecord:=Data;		// Get pointer to data
    Reply.ReportNo:=ReportID;	// Save the reply ID
    Reply.IORecord:=IORecord^;	// Transfer to our buffer
    Replied:=TRUE;	       	// Flag that we got reply
  END;
END;

{Error handler, just log it}
PROCEDURE THIDForm.AVRError(HidDev: TJvHidDevice; Error: DWORD);

BEGIN;
  MsgFormat('AVR Reply error : %.8X',[Error]);
END;

{Get the IN,DDR and OUT values for the specified AVR port}
PROCEDURE THIDForm.GETAVRPortValues(PortNo	: BYTE;   	// Port not to retrieve
				    VAR	PortIO	: TAVRPort);	// Buffer for port data

VAR	IORecord        : TAVRReport;	// Temp buffer for IO.
     	PortStr		: STRING;	// Temp string for probe report

BEGIN;
  PortStr:=Format('Checking port %s',[CHR(PortNo+ORD('A'))]);

  IORecord.ReportNo:=HIDReportPortOut;			// Set report to send
  WITH IORecord.IORecord.Port DO
  BEGIN;
    DestByte:=DestPort;
    CmdByte:=(PortNo AND PortMask) OR APIN;		// Build the command byte
    DataByte:=0;					// Zero data byte, doesn't actually matter as we are reading

    AVRIO(IORecord,HIDReportPortOut);			// Perform the IO
    IF ((CmdByte AND ErrorMask)<>ErrorMask) THEN 	// Check for error
    BEGIN;
      PortIO.PortIN:=DataByte;				// No error save PIN value

      PortStr:=PortStr+' Found';

      CmdByte:=(PortNo AND PortMask) OR APDDR;		// Retrieve DDR
      AVRIO(IORecord,HIDReportPortOut);
      PortIO.PortDDR:=DataByte;

      CmdByte:=(PortNo AND PortMask) OR APOUT;		// Retrieve OUT
      AVRIO(IORecord,HIDReportPortOut);
      PortIO.PortOUT:=DataByte;

      PortIO.PortValid:=TRUE;				// Flag valid values returned
    END
    ELSE
    BEGIN;
      PortIO.PortValid:=FALSE;				// Error, mark invalid
      PortStr:=PortStr+' Not found';
    END;
  END;
  Msg(PortStr);
END;

{Initialise the ports, itterate over ports A..G, trying to retrieve values from AVR}
PROCEDURE THIDForm.InitPorts;

VAR	Idx		: INTEGER;	// Index counter

BEGIN;
  Msg('Probing for AVR');
  FOR Idx:=0 TO 6 DO			// Ports A..G
  BEGIN;
    GETAVRPortValues(Idx,AVRPorts[Idx]); // Get port values into port array
  END;

  // Initialise the port dropdown on the form, only filling in the valid ports
  PortCombo.Items.Clear;
  FOR Idx:=0 TO 6 DO
  BEGIN;
    IF (AVRPorts[Idx].PortValid) THEN
      PortCombo.Items.Add(CHR(ORD('A')+Idx));
  END;
  PortCombo.ItemIndex:=0;

  // Tell the user what ports are available.
  Msg('AVR found, the following ports are available :');
  Msg(PortCombo.Items.CommaText);
END;

PROCEDURE THIDForm.InitMem;

VAR	IOReport        : TFeatureIO;	// Temp buffer for IO.

BEGIN;
  MaxEEPROM:=0;
  MaxRAM:=0;

  FillMemory(@IOReport,SizeOf(IOReport),0);
  IOReport.ReportID:=1;

  IF (Assigned(AVR)) THEN
  BEGIN;
    IF (AVR.GetFeature(IOReport,SizeOf(IOReport))) THEN
    BEGIN;
      Msg('GetFeature Success');
      WITH IOReport DO
      BEGIN;
        MsgFormat('Cmd=%2.2X, EESize=%4.4X, RAMSize=%4.4X',[CmdByte,EESize,RamSize]);
        MaxEEPROM:=EESize;
        MaxRAM:=RamSize;
      END;
    END
    ELSE
      Msg('GetFeature failed');
  END;
END;

{This is triggered when a new HID device is plugged in}
procedure THIDForm.HidCtrlArrival(HidDev: TJvHidDevice);
begin
  MsgFormat('HID Device found : %s',[HidDev.ProductName]);
end;

{Initialise the main form}
procedure THIDForm.FormCreate(Sender: TObject);
begin
  Initialized:=FALSE;
  FreeAVR;
end;

{Free the AVR USB channel if the AVR HID device removed}
procedure THIDForm.HidCtrlRemoval(HidDev: TJvHidDevice);
begin
  IF (HidDev.ProductName=MyUSB) THEN
  BEGIN;
    FreeAVR;
  END;
end;

{Called at program startup, and when a device change notification occours}
procedure THIDForm.HidCtrlDeviceChange(Sender: TObject);

VAR	Dev	: TJvHidDevice;		// Temp buffer for currently checked device

begin
  WHILE HidCtrl.CheckOut(Dev) DO	// Itterate over HID devices
    IF (Dev.ProductName=MyUSB) THEN	// If it is an AVR HID, then set it up
      AssignAVR(Dev);
end;

{Utility procedures to display messages in the output log}
PROCEDURE THIDForm.Msg(ToShow	: STRING);

BEGIN;
  AvrMemo.Lines.Add(ToShow);
END;

PROCEDURE THIDForm.MsgFormat(ToShow	: STRING;
    			     Params	: ARRAY OF CONST);

BEGIN;
  Msg(Format(ToShow,Params));
END;

{Setup the IO record ready to write to the USB HID, taking values}
{as selected by the user}
PROCEDURE THIDForm.SetupPortIO(VAR AVRIORecord	: TAVRReport;	// Record to build
			       IsWrite		: BOOLEAN);   	// True if we are writing

BEGIN;
  AVRIORecord.ReportNo:=HIDReportPortOut;			// Set report to send
  WITH AVRIORecord.IORecord.Port DO
  BEGIN;
    CmdByte:=(ORD(PortCombo.Text[1])-ORD('A')) AND PortMask;	// Enter the port
    CmdByte:=CmdByte+(RegCombo.ItemIndex SHL RegShift);  	// And the register  offset
    IF (IsWrite) THEN                                      	// If this is a write
    BEGIN;
      CmdByte:=CmdByte OR WriteMask;                     	// Or in the write mask
      DataByte:=StrToInt('$'+Trim(Value.Text));			// And set the data byte
    END;
    DestByte:=DestPort;						// Flag destination is port
  END;
END;

{Executed when the user clicks the read button}
procedure THIDForm.ReadPortBtnClick(Sender: TObject);

VAR	IORequest	: TAVRReport;		// Temp IO request

begin
  MsgFormat('Reading port : %s, reg %s',[PortCombo.Text,RegCombo.Text]);
  SetupPortIO(IORequest,FALSE);              	// Setup the request
  AVRIO(IORequest);                      	// Execute it

  WITH (IORequest.IORecord.Port) DO
  BEGIN;
    IF ((CmdByte AND ErrorMask)<>ErrorMask) THEN
    BEGIN;                                	// If no error display in log
      Value.Text:=Format('%2.2X',[DataByte]);
      MsgFormat('Value=%2.2X',[DataByte]);
    END
    ELSE
    BEGIN;                                	// Otherwise display error
      MsgFormat('I/O Error %2.2X reading port',[DataByte]);
    END;
  END;
end;

{Executed when the user clicks the write button}
procedure THIDForm.WritePortBtnClick(Sender: TObject);

VAR	IORequest	: TAVRReport;		// Temp IO request

begin
  MsgFormat('Writing %s to port %s, reg %s',[Value.Text,PortCombo.Text,RegCombo.Text]);
  SetupPortIO(IORequest,TRUE);                  // Setup the request
  AVRIO(IORequest);                             // Execute it

  WITH IORequest.IORecord.Port DO
  BEGIN;
    IF ((CmdByte AND ErrorMask)<>ErrorMask) THEN 	// display results or error
      MsgFormat('Value=%2.2X written',[DataByte])
    ELSE
      MsgFormat('I/O Error %2.2X writing port',[DataByte]);
  END;
end;

{Executed when the form is shown}
procedure THIDForm.FormShow(Sender: TObject);
begin
  IF (NOT Initialized) THEN
  BEGIN;
    AvrMemo.Lines.Clear;  	// Clear the log
    RegCombo.ItemIndex:=0;	// Initialise the AVR
    Msg(Signon);
    Msg('');
    IF (Assigned(AVR)) THEN
    BEGIN;
      Msg('');
      Msg('AVR found, the following ports are available :');
      AvrMemo.Lines.AddStrings(PortCombo.Items);
    END;
  END;
end;

PROCEDURE THIDForm.SetupControls(State	: BOOLEAN);

BEGIN;
  PortCombo.Enabled:=State;
  RegCombo.Enabled:=State;
  Value.Enabled:=State;
  ReadPortBtn.Enabled:=State;
  WritePortBtn.Enabled:=State;
  EERead.Enabled:=State;
  EEWrite.Enabled:=State;
  RAMRead.Enabled:=State;
  RAMWrite.Enabled:=State;
  IF (State=TRUE) THEN
  BEGIN;
    EEAddress.Text:=Format('%4.4X',[0]);
    RAMAddress.Text:=Format('%4.4X',[0]);
  END;
END;

PROCEDURE THIdForm.SetupEE_RAM_IO(VAR AVRIORecord	: TAVRReport;
    		      	          IsWrite		: BOOLEAN;
                                  Dest			: BYTE);

BEGIN;
  WITH AVRIORecord.IORecord.EEPROM_RAM DO
  BEGIN;
    CmdByte:=ReadOrWrite;		// Read or write Dest
    IF (IsWrite) THEN          		// Mask in write if needed
      CmdByte:=CmdByte OR WriteMask;

    DestByte:=Dest;			// Destination
    IF (Dest=DestEEPROM) THEN
    BEGIN;
      Address:=StrToInt('$'+Trim(EEAddress.Text));
      DataByte:=StrToInt('$'+Trim(EEValue.Text));
    END
    ELSE
    BEGIN;
      Address:=StrToInt('$'+Trim(RAMAddress.Text));
      DataByte:=StrToInt('$'+Trim(RAMValue.Text));
    END
  END;
END;

PROCEDURE THIDForm.EE_RAM_ReadWrite(IsWrite	: BOOLEAN;
				    Dest	: BYTE);

VAR	IOReport	: TAVRReport;
	DestStr		: STRING;

BEGIN;
  IF (Dest=DestEEPROM) THEN
    DestStr:='EEPROM'
  ELSE
    DestStr:='RAM';

  SetupEE_RAM_IO(IOReport,IsWrite,Dest);
  AVRIO(IOReport,HIDReportEEPROMOut);

  WITH IOReport.IORecord.EEPROM_RAM DO
  BEGIN;
    IF ((CmdByte AND ErrorMask)<>ErrorMask) THEN 	// Check for error
    BEGIN;
      IF ((CmdByte AND WriteMask)<>WriteMask) THEN
      BEGIN;
        MsgFormat('%s %4.4X=%2.2X',[DestStr,Address,DataByte]);
        IF (Dest=DestEEPROM) THEN
          EEValue.Text:=Format('%2.2X',[DataByte])
        ELSE
          RAMValue.Text:=Format('%2.2X',[DataByte]);
      END
      ELSE
        MsgFormat('Wrote %2.2X to %s at %4.4X',[DataByte,DestStr,Address]);
    END
    ELSE
      MsgFormat('Error %2.2X accessing %s at %4.4X',[DataByte,DestStr,Address]);
  END;
END;

procedure THIDForm.EEReadClick(Sender: TObject);

begin
  MsgFormat('Reading from EEPROM at address : %s',[EEAddress.Text]);
  EE_RAM_ReadWrite(FALSE,DestEEPROM);
end;

procedure THIDForm.EEWriteClick(Sender: TObject);
begin
  MsgFormat('Writing %s to EEPROM at address : %s',[EEValue.Text,EEAddress.Text]);
  EE_RAM_ReadWrite(TRUE,DestEEPROM);
end;

procedure THIDForm.RAMReadClick(Sender: TObject);
begin
  MsgFormat('Reading from RAM at address : %s',[EEAddress.Text]);
  EE_RAM_ReadWrite(FALSE,DestRAM);
end;

procedure THIDForm.RAMWriteClick(Sender: TObject);
begin
  MsgFormat('Writing %s to RAM at address : %s',[EEValue.Text,EEAddress.Text]);
  EE_RAM_ReadWrite(TRUE,DestRAM);
end;

procedure THIDForm.GetFeatureBtnClick(Sender: TObject);

VAR	IOReport	: TFeatureIO;

begin
  FillMemory(@IOReport,SizeOf(IOReport),0);
  IOReport.ReportID:=1;

  IF (Assigned(AVR)) THEN
  BEGIN;
    IF (AVR.GetFeature(IOReport,SizeOf(IOReport))) THEN
      Msg('GetFeature Success')
    ELSE
      Msg('GetFeature failed');

    WITH IOReport DO
      MsgFormat('Cmd=%2.2X, EESize=%4.4X, RAMSize=%4.4X',[CmdByte,EESize,RamSize]);
  END;
end;

procedure THIDForm.SetFeatureBtnClick(Sender: TObject);

VAR	IOReport	: TFeatureIO;

begin

  WITH IOReport DO
  BEGIN;
    ReportID:=1;
    CmdByte:=$00;
    EESize:=$2112;
    RamSize:=$6809;
  END;;

  IF (Assigned(AVR)) THEN
  BEGIN;
    IF (AVR.SetFeature(IOReport,SizeOf(IOReport))) THEN
      Msg('SetFeature Success')
    ELSE
      Msg('SetFeature failed');

  END;
end;

procedure THIDForm.Button1Click(Sender: TObject);

CONST	JoySel	= $04;

begin
  DoAVRIO(DDRC,JoySel,TRUE);
  DoAVRIO(PORTC,JoySel,TRUE);
  REPEAT;
    DoAVRIO(PinB,$00,FALSE);
    Application.ProcessMessages;
  UNTIL (FALSE);
end;

end.
