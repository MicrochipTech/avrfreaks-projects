unit fusebits;

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}
{$H+}

interface

uses
  Classes, SysUtils, LResources, Forms, Controls, Graphics, Dialogs, StdCtrls,
  DateUtils, ExtCtrls, libusb, intel_hex;

type

  { TForm2 }

  TForm2 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    BtTulisFuse: TButton;
    BtBacaFuse: TButton;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    PanelFuse: TPanel;
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    
    procedure CreateLFuse(jenisMK: string; jml_cb: integer);
    procedure CreateHFuse(jenisMK: string; jml_cb: integer);
    procedure CreateEFuse(jenisMK: string; jml_cb: integer);
    procedure InisialisasiFuse(jenisMK: string);
    procedure PembagianLFuse(tmpL: Byte);
    procedure PembagianHFuse(tmpL: Byte);
    procedure PembagianEFuse(tmpL: Byte);

    procedure TulisFuse(Sender: TObject);

    procedure BacaFuse(Sender: TObject);

    procedure LFUSEClick(Sender: TObject);
    procedure HFUSEClick(Sender: TObject);
    procedure EFUSEClick(Sender: TObject);
  private
    { private declarations }
    //Array checkbox
    cbLFuse: packed array of TCheckBox;
    cbHFuse: packed array of TCheckBox;
    cbEFuse: packed array of TCheckBox;

    //Kosongkan memori
    procedure FreeCheckBox(cb: array of TCheckBox);
  public
    { public declarations }
  end;

  TTipeFUSE = packed record
    idx: array [0..7] of integer;
    Stats: array [0..7] of boolean;
    Caps: array [0..7] of string;
  end;

var
  LFUSE: TTipeFUSE;
  HFUSE: TTipeFUSE;
  EFUSE: TTipeFUSE;
  nilailfuse: Byte;
  nilaihfuse: Byte;
  nilaiefuse: Byte;
  nilai_lfuse: array [0..7] of Byte;
  nilai_hfuse: array [0..7] of Byte;
  nilai_efuse: array [0..7] of Byte;
  str_lfuse: string= '255';
  str_hfuse: string= '255';
  str_efuse: string= '255';

implementation

uses main;

{ TForm2 }

procedure TForm2.PembagianLFuse(tmpL: Byte);
var
  AsliL: Byte;
  SisaL: Byte;
begin
  AsliL:=tmpL;
  tmpL:=tmpL div 128;
  SisaL:=AsliL mod 128;
  if tmpL=1 then
    cbLFuse[7].Checked:= false
  else if tmpL=0 then
    cbLFuse[7].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 64;
  SisaL:=SisaL mod 64;
  if tmpL=1 then
    cbLFuse[6].Checked:= false
  else if tmpL=0 then
    cbLFuse[6].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 32;
  SisaL:=SisaL mod 32;
  if tmpL=1 then
    cbLFuse[5].Checked:= false
  else if tmpL=0 then
    cbLFuse[5].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 16;
  SisaL:=SisaL mod 16;
  if tmpL=1 then
    cbLFuse[4].Checked:= false
  else if tmpL=0 then
    cbLFuse[4].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 8;
  SisaL:=SisaL mod 8;
  if tmpL=1 then
    cbLFuse[3].Checked:= false
  else if tmpL=0 then
    cbLFuse[3].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 4;
  SisaL:=SisaL mod 4;
  if tmpL=1 then
    cbLFuse[2].Checked:= false
  else if tmpL=0 then
    cbLFuse[2].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 2;
  SisaL:=SisaL mod 2;
  if tmpL=1 then
    cbLFuse[1].Checked:= false
  else if tmpL=0 then
    cbLFuse[1].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 1;
  SisaL:=SisaL mod 1;
  if tmpL=1 then
    cbLFuse[0].Checked:= false
  else if tmpL=0 then
    cbLFuse[0].Checked:= true;
end;

procedure TForm2.PembagianHFuse(tmpL: Byte);
var
  AsliL: Byte;
  SisaL: Byte;
begin
  AsliL:=tmpL;
  tmpL:=tmpL div 128;
  SisaL:=AsliL mod 128;
  if tmpL=1 then
    cbHFuse[7].Checked:= false
  else if tmpL=0 then
    cbHFuse[7].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 64;
  SisaL:=SisaL mod 64;
  if tmpL=1 then
    cbHFuse[6].Checked:= false
  else if tmpL=0 then
    cbHFuse[6].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 32;
  SisaL:=SisaL mod 32;
  if tmpL=1 then
    cbHFuse[5].Checked:= false
  else if tmpL=0 then
    cbHFuse[5].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 16;
  SisaL:=SisaL mod 16;
  if tmpL=1 then
    cbHFuse[4].Checked:= false
  else if tmpL=0 then
    cbHFuse[4].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 8;
  SisaL:=SisaL mod 8;
  if tmpL=1 then
    cbHFuse[3].Checked:= false
  else if tmpL=0 then
    cbHFuse[3].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 4;
  SisaL:=SisaL mod 4;
  if tmpL=1 then
    cbHFuse[2].Checked:= false
  else if tmpL=0 then
    cbHFuse[2].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 2;
  SisaL:=SisaL mod 2;
  if tmpL=1 then
    cbHFuse[1].Checked:= false
  else if tmpL=0 then
    cbHFuse[1].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 1;
  SisaL:=SisaL mod 1;
  if tmpL=1 then
    cbHFuse[0].Checked:= false
  else if tmpL=0 then
    cbHFuse[0].Checked:= true;
end;

procedure TForm2.PembagianEFuse(tmpL: Byte);
var
  AsliL: Byte;
  SisaL: Byte;
begin
  AsliL:=tmpL;
  tmpL:=tmpL div 128;
  SisaL:=AsliL mod 128;
  if tmpL=1 then
    cbEFuse[7].Checked:= false
  else if tmpL=0 then
    cbEFuse[7].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 64;
  SisaL:=SisaL mod 64;
  if tmpL=1 then
    cbEFuse[6].Checked:= false
  else if tmpL=0 then
    cbEFuse[6].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 32;
  SisaL:=SisaL mod 32;
  if tmpL=1 then
    cbEFuse[5].Checked:= false
  else if tmpL=0 then
    cbEFuse[5].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 16;
  SisaL:=SisaL mod 16;
  if tmpL=1 then
    cbEFuse[4].Checked:= false
  else if tmpL=0 then
    cbEFuse[4].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 8;
  SisaL:=SisaL mod 8;
  if tmpL=1 then
    cbEFuse[3].Checked:= false
  else if tmpL=0 then
    cbEFuse[3].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 4;
  SisaL:=SisaL mod 4;
  if tmpL=1 then
    cbEFuse[2].Checked:= false
  else if tmpL=0 then
    cbEFuse[2].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 2;
  SisaL:=SisaL mod 2;
  if tmpL=1 then
    cbEFuse[1].Checked:= false
  else if tmpL=0 then
    cbEFuse[1].Checked:= true;

  tmpL:=SisaL;
  tmpL:=tmpL div 1;
  SisaL:=SisaL mod 1;
  if tmpL=1 then
    cbEFuse[0].Checked:= false
  else if tmpL=0 then
    cbEFuse[0].Checked:= true;
end;

procedure TForm2.TulisFuse(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, j: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  waktu: TDateTime;
begin
  PanelFuse.Visible:= false;
  IDPerintah:= OPERASI_TULIS_FUSEBITS;
  
  //Tulis fuse, tetapi deteksi dahulu sizeOf dari semua jenis record fuse
  //Record yang kosong atau tidak diinisialisasikan tidak perlu dilakukan aksi
  if Assigned(cbEFuse) and Assigned(cbHFuse) and Assigned(cbLFuse) then
  begin
    str_lfuse:= IntToHex(nilailfuse, 2);
    str_hfuse:= IntToHex(nilaihfuse, 2);
    str_efuse:= IntToHex(nilaiefuse, 2);

    isiHex:= '027C'+str_lfuse+'7C'+str_hfuse+'7C'+str_efuse;
  end
  else if (not Assigned(cbEFuse)) and Assigned(cbHFuse) and Assigned(cbLFuse) then
  begin
    str_lfuse:= IntToHex(nilailfuse, 2);
    str_hfuse:= IntToHex(nilaihfuse, 2);

    isiHex:= '017C'+str_lfuse+'7C'+str_hfuse;
  end
  else if (not Assigned(cbEFuse)) and (not Assigned(cbHFuse)) and Assigned(cbLFuse) then
  begin
    str_lfuse:= IntToHex(nilailfuse, 2);

    isiHex:= '007C'+str_lfuse;
  end
  else if (not Assigned(cbLFuse)) and (not Assigned(cbHFuse)) and (not Assigned(cbEFuse)) then
  begin
    BtTulisFuse.Enabled:= false;
    BtBacaFuse.Enabled:= false;
  end;
  
  if Form1.tesEcho(StrToInt('$8888')) then
  begin
    if Form1.kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          j:= 0;
          panjangData:= Length(isiHex) div 2;
          
          while j < panjangData do
          begin
            dataHex:= Copy(isiHex, i+1, 2);
            buffer[j]:= StrToInt('$'+dataHex);
            Form1.Memo.Lines.add(IntToStr(i)+': '+(dataHex));
            Inc(j);
            i:= i+2;
          end;
          nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
          if nilaiKembalianUSB = j then
          begin
            nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

            Form1.Memo.Lines.add('Buffer telah penuh!');
          end;

          if Form1.kirimPaketAkhir then
          begin
            if Form1.inisialisasiUlang then
              ShowMessage('Proses pengiriman selesai,'#13#10't= '+IntToStr(SecondsBetween(waktu, Now))+' detik');
          end;

          usb_close(pointerDeviceUSB);
        end;
      end
      else
        ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
    end
    else
      ShowMessage('Device IRDownloader tidak merespons! Re-plug device IRDownloader!');
  end;

  PanelFuse.Visible:= true;
end;

procedure TForm2.BacaFuse(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, k: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  dataHex: string;
  waktu: TDateTime;
  masihTerima: Boolean;
begin
  PanelFuse.Visible:= false;
  IDPerintah:= OPERASI_BACA_FUSEBITS;
  if Form1.tesEcho(StrToInt('$8888')) then
  begin
    if Form1.kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          dataHex:= '';
          Form1.Memo.Lines.add('Data hex:');
          masihTerima:= true;

          while masihTerima do
          begin
            nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
            while nilaiKembalianUSB < 1 do
            begin
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
              Application.ProcessMessages;
            end;

            if bufferBalasan = TUNGGU_TERIMA_TRANSMISI then
            begin
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_BACA_BUFFER, 0, 0, buffer, PANJANG_DATA_TRANSFER, 5000);
              while nilaiKembalianUSB <> PANJANG_DATA_TRANSFER do
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_BACA_BUFFER, 0, 0, buffer, PANJANG_DATA_TRANSFER, 5000);
                Application.ProcessMessages;
              end;

              Form1.Memo.Lines.add(IntToStr(nilaiKembalianUSB));
              i:= 0;
              while i < PANJANG_DATA_TRANSFER do
              begin
                k:= (buffer[i] shl 8) or buffer[i+1];
                dataHex:= dataHex+IntToHex(k, 4);
                i:= i+2;
              end;

              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
              while nilaiKembalianUSB <= 1 do
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                Application.ProcessMessages;
                if bufferBalasan = TUNGGU_KIRIM_HOST then
                  break;
              end;
            end
            else if bufferBalasan = SELESAI_TERIMA_TRANSMISI then
              masihTerima:= false;
          end;

          if Form1.inisialisasiUlang then
            ShowMessage('Proses penerimaan selesai,'#13#10't= '+IntToStr(SecondsBetween(waktu, Now))+' detik');

          // Update tampilan checkbox fusebits
          if Assigned(cbEFuse) and Assigned(cbHFuse) and Assigned(cbLFuse) then
          begin
            str_lfuse:= '$'+Copy(dataHex, 5, 2);
            str_hfuse:= '$'+Copy(dataHex, 9, 2);
            str_efuse:= '$'+Copy(dataHex, 13, 2);

            // Baca lfuse, hfuse dan efuse
            nilailfuse:= StrToInt(str_lfuse);
            nilaihfuse:= StrToInt(str_hfuse);
            nilaiefuse:= StrToInt(str_efuse);
            
            // Update tampilan lfuse, hfuse dan efuse
            PembagianLFuse(nilailfuse);
            PembagianHFuse(nilaihfuse);
            PembagianEFuse(nilaiefuse);
          end
          else if (not Assigned(cbEFuse)) and Assigned(cbHFuse) and Assigned(cbLFuse) then
          begin
            str_lfuse:= '$'+Copy(dataHex, 5, 2);
            str_hfuse:= '$'+Copy(dataHex, 9, 2);

            // Baca lfuse dan hfuse
            nilailfuse:= StrToInt(str_lfuse);
            nilaihfuse:= StrToInt(str_hfuse);
            
            // Update tampilan lfuse dan hfuse
            PembagianLFuse(nilailfuse);
            PembagianHFuse(nilaihfuse);
          end
          else if (not Assigned(cbEFuse)) and (not Assigned(cbHFuse)) and Assigned(cbLFuse) then
          begin
            str_lfuse:= '$'+Copy(dataHex, 5, 2);
            
            // Baca lfuse
            nilailfuse:= StrToInt(str_lfuse);
            
            // Update tampilan lfuse
            PembagianLFuse(nilailfuse);
          end
          else if (not Assigned(cbLFuse)) and (not Assigned(cbHFuse)) and (not Assigned(cbEFuse)) then
          begin
            BtTulisFuse.Enabled:= false;
            BtBacaFuse.Enabled:= false;
          end;

          usb_close(pointerDeviceUSB);
        end;
      end
      else
        ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
    end
    else
      ShowMessage('Device IRDownloader tidak merespons! Re-plug device IRDownloader!');
  end;
  PanelFuse.Visible:= true;
end;

procedure TForm2.LFUSEClick(Sender: TObject);
var
  i: integer;
begin
  if Sender is TCheckBox then
  begin
    if TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = LFUSE.Caps[i] then
          Break;
      end;
      nilai_lfuse[i]:= 0;
    end
    else if not TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = LFUSE.Caps[i] then
          Break;
      end;
      nilai_lfuse[i]:= LFUSE.idx[i];
    end;
    nilailfuse:= nilai_lfuse[7]+nilai_lfuse[6]+nilai_lfuse[5]+nilai_lfuse[4]+nilai_lfuse[3]+nilai_lfuse[2]+nilai_lfuse[1]+nilai_lfuse[0];
  end;
end;

procedure TForm2.HFUSEClick(Sender: TObject);
var
  i: integer;
begin
  if Sender is TCheckBox then
  begin
    if TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = HFUSE.Caps[i] then
          Break;
      end;
      nilai_hfuse[i]:= 0;
    end
    else if not TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = HFUSE.Caps[i] then
          Break;
      end;
      nilai_hfuse[i]:= HFUSE.idx[i];
    end;
    nilaihfuse:= nilai_hfuse[7]+nilai_hfuse[6]+nilai_hfuse[5]+nilai_hfuse[4]+nilai_hfuse[3]+nilai_hfuse[2]+nilai_hfuse[1]+nilai_hfuse[0];
  end;
end;

procedure TForm2.EFUSEClick(Sender: TObject);
var
  i: integer;
begin
  if Sender is TCheckBox then
  begin
    if TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = EFUSE.Caps[i] then
          Break;
      end;
      nilai_efuse[i]:= 0;
    end
    else if not TCheckBox(Sender).Checked then
    begin
      for i:= 0 to 7 do
      begin
        if TCheckBox(Sender).Caption = EFUSE.Caps[i] then
          Break;
      end;
      nilai_efuse[i]:= EFUSE.idx[i];
    end;
    nilaiefuse:= nilai_efuse[7]+nilai_efuse[6]+nilai_efuse[5]+nilai_efuse[4]+nilai_efuse[3]+nilai_efuse[2]+nilai_efuse[1]+nilai_efuse[0];
  end;
end;

procedure TForm2.CreateLFuse(jenisMK: string; jml_cb: integer);
var
  jml: integer;
begin
  //Pertama, kosongkan memori
  FreeCheckBox(cbLFuse);

  SetLength(cbLFuse, jml_cb);

  //Buat check box baru
  for jml:= 0 to -1 + jml_cb do
  begin
    cbLFuse[jml]:= TCheckBox.Create(nil);

    //OnClick event
    cbLFuse[jml].OnClick:= LFUSEClick;

    cbLFuse[jml].Checked:= true;

    //enable or not
    cbLFuse[jml].Enabled:= LFUSE.Stats[jml];

    //assign caption
    cbLFuse[jml].Caption:= LFUSE.Caps[jml];

    cbLFuse[jml].Left:= panelFuse.Width - (cbLFuse[jml].Width+70*jml);
    cbLFuse[jml].Top:= 96;

    //Set parent, jika tidak maka check box tidak akan ditampilkan
    cbLFuse[jml].Parent:= panelFuse;
  end;
end;

procedure TForm2.CreateHFuse(jenisMK: string; jml_cb: integer);
var
  jml: integer;
begin
  //Pertama, kosongkan memori
  FreeCheckBox(cbHFuse);

  SetLength(cbHFuse, jml_cb);

  //Buat check box baru
  for jml:= 0 to -1 + jml_cb do
  begin
    cbHFuse[jml]:= TCheckBox.Create(nil);

    //OnClick event
    cbHFuse[jml].OnClick:= HFUSEClick;

    cbHFuse[jml].Checked:= true;

    //enable or not
    cbHFuse[jml].Enabled:= HFUSE.Stats[jml];

    //assign caption
    cbHFuse[jml].Caption:= HFUSE.Caps[jml];

    cbHFuse[jml].Left:= panelFuse.Width - (cbHFuse[jml].Width+70*jml);
    cbHFuse[jml].Top:= 60;

    //Set parent, jika tidak maka check box tidak akan ditampilkan
    cbHFuse[jml].Parent:= panelFuse;
  end;
end;

procedure TForm2.CreateEFuse(jenisMK: string; jml_cb: integer);
var
  jml: integer;
begin
  //Pertama, kosongkan memori
  FreeCheckBox(cbEFuse);

  SetLength(cbEFuse, jml_cb);

  //Buat check box baru
  for jml:= 0 to -1 + jml_cb do
  begin
    cbEFuse[jml]:= TCheckBox.Create(nil);

    //OnClick event
    cbEFuse[jml].OnClick:= EFUSEClick;

    cbEFuse[jml].Checked:= true;

    //enable or not
    cbEFuse[jml].Enabled:= EFUSE.Stats[jml];

    //assign caption
    cbEFuse[jml].Caption:= EFUSE.Caps[jml];

    cbEFuse[jml].Left:= panelFuse.Width - (cbEFuse[jml].Width+70*jml);
    cbEFuse[jml].Top:= 24;

    //Set parent, jika tidak maka check box tidak akan ditampilkan
    cbEFuse[jml].Parent:= panelFuse;
  end;
end;

procedure TForm2.FreeCheckBox(cb: array of TCheckBox);
var
  jml: integer;
begin
  for jml:= Low(cb) to High(cb) do
  begin
    cb[jml].Free;
    cb[jml]:= nil;
  end;
end;

procedure TForm2.InisialisasiFuse(jenisMK: string);
var
  i: integer;
begin
  if (jenisMK = 'AT90S2333') or (jenisMK = 'AT90S4433') then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'BODEN';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'BODLEVEL';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= 'SPIEN';
    LFUSE.Stats[6]:= false;
    LFUSE.Caps[6]:= '6';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= '7';

    CreateLFuse(jenisMK, 8);
  end
  else if (jenisMK = 'AT90S4434') or (jenisMK = 'AT90S8535') then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'FSTRT';
    LFUSE.Stats[1]:= false;
    LFUSE.Caps[1]:= '1';
    LFUSE.Stats[2]:= false;
    LFUSE.Caps[2]:= '2';
    LFUSE.Stats[3]:= false;
    LFUSE.Caps[3]:= '3';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= '4';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= '5';
    LFUSE.Stats[6]:= false;
    LFUSE.Caps[6]:= '6';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= '7';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'AT90S2343' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'RCEN';
    LFUSE.Stats[1]:= false;
    LFUSE.Caps[1]:= '1';
    LFUSE.Stats[2]:= false;
    LFUSE.Caps[2]:= '2';
    LFUSE.Stats[3]:= false;
    LFUSE.Caps[3]:= '3';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= '4';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= '5';
    LFUSE.Stats[6]:= false;
    LFUSE.Caps[6]:= '6';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= '7';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega8' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'CKOPT';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'WDTON';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= 'RSTDISBL';

    CreateHFuse(jenisMK, 8);
  end
  else if (jenisMK = 'ATmega16') or (jenisMK = 'ATmega32') then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'CKOPT';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'JTAGEN';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= 'OCDEN';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega48' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOUT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'CKDIV8';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BODLEVEL0';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BODLEVEL1';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BODLEVEL2';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'WDTON';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'DWEN';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= 'RSTDISBL';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= true;
    EFUSE.Caps[0]:= 'SELFPRGEN';
    EFUSE.Stats[1]:= false;
    EFUSE.Caps[1]:= '1';
    EFUSE.Stats[2]:= false;
    EFUSE.Caps[2]:= '2';
    EFUSE.Stats[3]:= false;
    EFUSE.Caps[3]:= '3';
    EFUSE.Stats[4]:= false;
    EFUSE.Caps[4]:= '4';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end
  else if (jenisMK = 'ATmega64') or (jenisMK = 'ATmega128') then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'CKOPT';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'JTAGEN';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= 'OCDEN';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= true;
    EFUSE.Caps[0]:= 'WDTON';
    EFUSE.Stats[1]:= true;
    EFUSE.Caps[1]:= 'M103C';
    EFUSE.Stats[2]:= false;
    EFUSE.Caps[2]:= '2';
    EFUSE.Stats[3]:= false;
    EFUSE.Caps[3]:= '3';
    EFUSE.Stats[4]:= false;
    EFUSE.Caps[4]:= '4';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega88' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOUT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'CKDIV8';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BODLEVEL0';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BODLEVEL1';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BODLEVEL2';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'WDTON';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'DWEN';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= 'RSTDISBL';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= true;
    EFUSE.Caps[0]:= 'BOOTRST';
    EFUSE.Stats[1]:= true;
    EFUSE.Caps[1]:= 'BOOTSZ0';
    EFUSE.Stats[2]:= true;
    EFUSE.Caps[2]:= 'BOOTSZ1';
    EFUSE.Stats[3]:= false;
    EFUSE.Caps[3]:= '3';
    EFUSE.Stats[4]:= false;
    EFUSE.Caps[4]:= '4';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega103' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'SUT0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'SUT1';
    LFUSE.Stats[2]:= false;
    LFUSE.Caps[2]:= '2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'EESAVE';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= '4';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= '5';
    LFUSE.Stats[6]:= false;
    LFUSE.Caps[6]:= '6';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= '7';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega161' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'BODEN';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'BODLEVEL';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SPIEN';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BOOTRST';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= '7';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega162' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOUT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'CKDIV8';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'WDTON';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'JTAGEN';
    HFUSE.Stats[7]:= true;
    HFUSE.Caps[7]:= 'OCDEN';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= false;
    EFUSE.Caps[0]:= '0';
    EFUSE.Stats[1]:= true;
    EFUSE.Caps[1]:= 'BODLEVEL';
    EFUSE.Stats[2]:= true;
    EFUSE.Caps[2]:= 'BOD1LEVEL';
    EFUSE.Stats[3]:= true;
    EFUSE.Caps[3]:= 'BOD2LEVEL';
    EFUSE.Stats[4]:= true;
    EFUSE.Caps[4]:= 'M161C';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega163' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= '4';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= '5';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= false;
    HFUSE.Caps[3]:= '3';
    HFUSE.Stats[4]:= false;
    HFUSE.Caps[4]:= '4';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= '5';
    HFUSE.Stats[6]:= false;
    HFUSE.Caps[6]:= '6';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= '7';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega169' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOUT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'CKDIV8';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'WDTON';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'JTAGEN';
    HFUSE.Stats[7]:= true;
    HFUSE.Caps[7]:= 'OCDEN';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= false;
    EFUSE.Caps[0]:= 'RSTDISBL';
    EFUSE.Stats[1]:= true;
    EFUSE.Caps[1]:= 'BODLEVEL';
    EFUSE.Stats[2]:= true;
    EFUSE.Caps[2]:= 'BOD1LEVEL';
    EFUSE.Stats[3]:= true;
    EFUSE.Caps[3]:= 'BOD2LEVEL';
    EFUSE.Stats[4]:= false;
    EFUSE.Caps[4]:= '4';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega8515' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'CKOPT';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'WDTON';
    HFUSE.Stats[7]:= true;
    HFUSE.Caps[7]:= 'S8515C';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATmega8535' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BOOTRST';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BOOTSZ0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BOOTSZ1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'EESAVE';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'CKOPT';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'WDTON';
    HFUSE.Stats[7]:= true;
    HFUSE.Caps[7]:= 'S8535C';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATtiny12' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= 'RSTDISBL';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= 'SPIEN';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATtiny13' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'SUT0';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'SUT1';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'CKDIV8';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'WDTON';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'EESAVE';
    LFUSE.Stats[7]:= false;
    LFUSE.Caps[7]:= 'SPIEN';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= false;
    HFUSE.Caps[0]:= 'RSTDISBL';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BODLEVEL0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BODLEVEL1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'DWEN';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'SELFPRGEN';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= '5';
    HFUSE.Stats[6]:= false;
    HFUSE.Caps[6]:= '6';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= '7';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATtiny15' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= false;
    LFUSE.Caps[2]:= '2';
    LFUSE.Stats[3]:= false;
    LFUSE.Caps[3]:= '3';
    LFUSE.Stats[4]:= false;
    LFUSE.Caps[4]:= 'RSTDISBL';
    LFUSE.Stats[5]:= false;
    LFUSE.Caps[5]:= 'SPIEN';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'BODEN';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'BODLEVEL';

    CreateLFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATtiny26' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOPT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'PLLCK';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= true;
    HFUSE.Caps[0]:= 'BODEN';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BODLEVEL';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'EESAVE';
    HFUSE.Stats[3]:= false;
    HFUSE.Caps[3]:= 'SPIEN';
    HFUSE.Stats[4]:= false;
    HFUSE.Caps[4]:= 'RSTDISBL';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= '5';
    HFUSE.Stats[6]:= false;
    HFUSE.Caps[6]:= '6';
    HFUSE.Stats[7]:= false;
    HFUSE.Caps[7]:= '7';

    CreateHFuse(jenisMK, 8);
  end
  else if jenisMK = 'ATtiny2313' then
  begin
    //LFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        LFUSE.idx[i]:= 1
      else if i = 1 then
        LFUSE.idx[i]:= 2
      else if i = 2 then
        LFUSE.idx[i]:= 4
      else if i = 3 then
        LFUSE.idx[i]:= 8
      else if i = 4 then
        LFUSE.idx[i]:= 16
      else if i = 5 then
        LFUSE.idx[i]:= 32
      else if i = 6 then
        LFUSE.idx[i]:= 64
      else if i = 7 then
        LFUSE.idx[i]:= 128;
    end;
    LFUSE.Stats[0]:= true;
    LFUSE.Caps[0]:= 'CKSEL0';
    LFUSE.Stats[1]:= true;
    LFUSE.Caps[1]:= 'CKSEL1';
    LFUSE.Stats[2]:= true;
    LFUSE.Caps[2]:= 'CKSEL2';
    LFUSE.Stats[3]:= true;
    LFUSE.Caps[3]:= 'CKSEL3';
    LFUSE.Stats[4]:= true;
    LFUSE.Caps[4]:= 'SUT0';
    LFUSE.Stats[5]:= true;
    LFUSE.Caps[5]:= 'SUT1';
    LFUSE.Stats[6]:= true;
    LFUSE.Caps[6]:= 'CKOUT';
    LFUSE.Stats[7]:= true;
    LFUSE.Caps[7]:= 'CKDIV8';

    CreateLFuse(jenisMK, 8);

    //HFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        HFUSE.idx[i]:= 1
      else if i = 1 then
        HFUSE.idx[i]:= 2
      else if i = 2 then
        HFUSE.idx[i]:= 4
      else if i = 3 then
        HFUSE.idx[i]:= 8
      else if i = 4 then
        HFUSE.idx[i]:= 16
      else if i = 5 then
        HFUSE.idx[i]:= 32
      else if i = 6 then
        HFUSE.idx[i]:= 64
      else if i = 7 then
        HFUSE.idx[i]:= 128;
    end;
    HFUSE.Stats[0]:= false;
    HFUSE.Caps[0]:= 'RSTDISBL';
    HFUSE.Stats[1]:= true;
    HFUSE.Caps[1]:= 'BODLEVEL0';
    HFUSE.Stats[2]:= true;
    HFUSE.Caps[2]:= 'BODLEVEL1';
    HFUSE.Stats[3]:= true;
    HFUSE.Caps[3]:= 'BODLEVEL2';
    HFUSE.Stats[4]:= true;
    HFUSE.Caps[4]:= 'WDTON';
    HFUSE.Stats[5]:= false;
    HFUSE.Caps[5]:= 'SPIEN';
    HFUSE.Stats[6]:= true;
    HFUSE.Caps[6]:= 'EESAVE';
    HFUSE.Stats[7]:= true;
    HFUSE.Caps[7]:= 'DWEN';

    CreateHFuse(jenisMK, 8);

    //EFUSE
    //Indexing
    for i:= 0 to 7 do
    begin
      if i = 0 then
        EFUSE.idx[i]:= 1
      else if i = 1 then
        EFUSE.idx[i]:= 2
      else if i = 2 then
        EFUSE.idx[i]:= 4
      else if i = 3 then
        EFUSE.idx[i]:= 8
      else if i = 4 then
        EFUSE.idx[i]:= 16
      else if i = 5 then
        EFUSE.idx[i]:= 32
      else if i = 6 then
        EFUSE.idx[i]:= 64
      else if i = 7 then
        EFUSE.idx[i]:= 128;
    end;
    EFUSE.Stats[0]:= true;
    EFUSE.Caps[0]:= 'SPMEN';
    EFUSE.Stats[1]:= false;
    EFUSE.Caps[1]:= '1';
    EFUSE.Stats[2]:= false;
    EFUSE.Caps[2]:= '2';
    EFUSE.Stats[3]:= false;
    EFUSE.Caps[3]:= '3';
    EFUSE.Stats[4]:= false;
    EFUSE.Caps[4]:= '4';
    EFUSE.Stats[5]:= false;
    EFUSE.Caps[5]:= '5';
    EFUSE.Stats[6]:= false;
    EFUSE.Caps[6]:= '6';
    EFUSE.Stats[7]:= false;
    EFUSE.Caps[7]:= '7';

    CreateEFuse(jenisMK, 8);
  end;
end;

procedure TForm2.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
  FreeCheckBox(cbLFuse);
  FreeCheckBox(cbHFuse);
  FreeCheckBox(cbEFuse);
  
  CloseAction:= caFree;
end;

procedure TForm2.FormCreate(Sender: TObject);
begin
  nilailfuse:= 255;
  nilaihfuse:= 255;
  nilaiefuse:= 255;
  str_lfuse:= '255';
  str_hfuse:= '255';
  str_efuse:= '255';
  
  InisialisasiFuse(tipeAVR);
  BacaFuse(nil);
end;

procedure TForm2.FormShow(Sender: TObject);
begin
  Label2.Caption:= 'Jenis mikrokontroler: '+tipeAVR;
end;

initialization
  {$I fusebits.lrs}

end.

