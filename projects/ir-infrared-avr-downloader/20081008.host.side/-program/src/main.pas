(* avrirdownloader host side by Hendy Eka H <hendyeka.hardana@gmail.com>
 * Copyright (c) 2008 UBU-4001 Electrical Engineering Brawijaya University Of Malang, Indonesia
 * License: GNU GPL v2 (see Copying.txt)
 *
 * LIBUSB-WIN32, Generic Windows USB Library
 * Copyright (c) 2002-2004 Stephan Meyer <ste_meyer@web.de>
 * Copyright (c) 2000-2004 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * LIBUSB-WIN32 translation
 * Copyright (c) 2004 Yvo Nelemans <ynlmns@xs4all.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *)

unit main;

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}
{$H+}

interface

uses
  Classes, SysUtils, LResources, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  Menus, StdCtrls, mmsystem, DateUtils, IniFiles, Buttons, ShellAPI, libusb,
  intel_hex;

type
  TDevObj = class(TObject)
  public
    Serial: string;
    deviceUSB: pusb_device;
  end;

  { TForm1 }

  TForm1 = class(TForm)
    BtTulisFlash: TButton;
    BtBacaFlash: TButton;
    BtReloadFlash: TButton;
    BtTulisEEP: TButton;
    BtBacaEEP: TButton;
    BtReloadEEP: TButton;
    BtHapusMemori: TButton;
    BtEcho: TButton;
    CmbJenisMK: TComboBox;
    EditEcho: TEdit;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    GroupBox3: TGroupBox;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label7: TLabel;
    LabelDeviceSignature: TLabel;
    LabelPanjangData: TLabel;
    MainMenu1: TMainMenu;
    Memo: TMemo;
    MemoDataHex: TMemo;
    MemoDataASCII: TMemo;
    MenuItem1: TMenuItem;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    MnFuseBits: TMenuItem;
    MnTulisEEP: TMenuItem;
    MnTulisFlash: TMenuItem;
    MnHapus: TMenuItem;
    MnAbout: TMenuItem;
    MnPetunjuk: TMenuItem;
    MnHelp: TMenuItem;
    MnCekDevice: TMenuItem;
    MnProgram: TMenuItem;
    MnFile: TMenuItem;
    MnLoadFlash: TMenuItem;
    MnLoadEEP: TMenuItem;
    MenuItem4: TMenuItem;
    MnSaveFlash: TMenuItem;
    MnSaveEEP: TMenuItem;
    MnExit: TMenuItem;
    MnCek: TMenuItem;
    OpenDialogEEPROM: TOpenDialog;
    OpenDialogFlash: TOpenDialog;
    Panel1: TPanel;
    Panel2: TPanel;
    Panel3: TPanel;
    Panel4: TPanel;
    Panel5: TPanel;
    SaveDialogFlash: TSaveDialog;
    SaveDialogEEPROM: TSaveDialog;
    TimerTimeout: TTimer;
    procedure BtBacaEEPClick(Sender: TObject);
    procedure BtHapusMemoriClick(Sender: TObject);
    procedure BtReloadEEPClick(Sender: TObject);
    procedure BtReloadFlashClick(Sender: TObject);
    procedure BtTulisEEPClick(Sender: TObject);
    procedure CmbJenisMKChange(Sender: TObject);
    procedure MnFuseBitsClick(Sender: TObject);
    procedure MnLoadEEPClick(Sender: TObject);
    procedure LogoObdevClick(Sender: TObject);
    procedure MnSaveEEPClick(Sender: TObject);
    procedure MnSaveFlashClick(Sender: TObject);
    function tesEcho(dataInput: Integer): Boolean;
    function kirimPaketAwal: Boolean;
    function kirimPaketAkhir: Boolean;
    function inisialisasiUlang: Boolean;
    procedure BtEchoClick(Sender: TObject);
    procedure MnLoadFlashClick(Sender: TObject);
    procedure BtBacaFlashClick(Sender: TObject);
    procedure BtTulisFlashClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MnCekDeviceClick(Sender: TObject);
    procedure MnExitClick(Sender: TObject);
    procedure TimerTimeoutTimer(Sender: TObject);
  private
    { private declarations }
    function cekDevice: Boolean;
  public
    { public declarations }
    procedure matikanFungsional;
    procedure aktifkanFungsional;
  end;

var
  Form1: TForm1;
  
  // Device USB
  statusDevice: Boolean;
  statusKoneksi: Boolean;
  statusTransmisi: Boolean;
  DevObj: TDevObj;

  // Transmisi
  jumlahPaket: integer;
  IDPerintah: integer;
  
  // File .ini
  fileIni: TIniFile;
  tipeAVR: string;
  deviceSignature: string;

const
  VENDORID = $16C0;
  PRODUCTID= $05DC;
  PRODUCT= 'IRDownloader';
  MANUFACTURER= 'UBU4001';
  
  PANJANG_DATA= 128;
  PANJANG_DATA_TRANSFER= 128;
  PANJANG_BUFFER_PAKET= 256;
  
  OPERASI_TULIS_FLASH= 70;// ASCII 'F'
  OPERASI_BACA_FLASH= 102;// ASCII 'f'
  OPERASI_TULIS_EEPROM= 69;// ASCII 'E'
  OPERASI_BACA_EEPROM= 101;// ASCII 'e'
  OPERASI_TULIS_FUSEBITS= 66;// ASCII 'B'
  OPERASI_BACA_FUSEBITS= 98;// ASCII 'b'
  OPERASI_HAPUS_CHIP= 72;// ASCII 'H'

  CMD_ECHO= 0;
  CMD_KIRIM_PERINTAH_TULIS= 1;
  CMD_KIRIM_PERINTAH_BACA= 2;
  CMD_ISI_BUFFER= 3;
  CMD_BACA_BUFFER= 4;
  CMD_KIRIM_AKHIR= 5;
  CMD_INISIALISASI= 6;
  CMD_CEK_STATUS_TRANSMISI= 7;

  KIRIM_PAKET_PERINTAH_TULIS= 1;
  KIRIM_PAKET_PERINTAH_BACA= 2;
  KIRIM_PAKET_PERINTAH_SELESAI= 3;
  ISI_BUFFER= 4;
  ISI_BUFFER_SELESAI= 5;
  KIRIM_PAKET_DATA= 6;
  KIRIM_PAKET_DATA_SELESAI= 7;
  KIRIM_AKHIR= 8;
  KIRIM_AKHIR_SELESAI= 9;
  PROSES_TRANSMISI= 10;
  TUNGGU_PROSES_TRANSMISI= 11;
  TERIMA_TRANSMISI= 12;
  TUNGGU_TERIMA_TRANSMISI= 13;
  TUNGGU_KIRIM_HOST= 14;
  SELESAI_TERIMA_TRANSMISI= 15;
  IDLE= 16;

  DevaisKePC= USB_ENDPOINT_IN;
  PCKeDevais= USB_ENDPOINT_OUT;

implementation

uses fusebits;

{ TForm1 }

function TForm1.cekDevice: Boolean;
var
  foundvendor, foundprodukt: boolean;
  buf: array[0..100] of char;
  bus: pusb_bus;
  l: integer;
  dev: pusb_device;
  pointerDeviceUSB: pointer;
  prodStr, manuf, serial: string;
begin
  statusDevice:= false;

  foundvendor:= false;
  foundprodukt:= false;
  usb_init;// Inisialisasi libusb
  Memo.Lines.Add('');
  Memo.Lines.Add(usb_strerror);
  
  if(usb_find_busses < 1) then
    statusDevice:= false;

  if(usb_find_devices < 1) then
    statusDevice:= false;

  bus:= usb_get_busses;
  if  bus = nil then
    statusDevice:= false;

  while Assigned(bus) do
  begin
    dev:= bus^.devices;
    while Assigned(dev) do
    begin
      if dev^.descriptor.idVendor = VENDORID  then
        foundvendor:= true;

      if dev^.descriptor.idProduct = PRODUCTID then
        foundprodukt:= true;

      if (foundvendor and foundprodukt) then
      begin
        Memo.Lines.add('-Device Obdev: '+bus.dirname+'/'+dev.filename);
        pointerDeviceUSB:= usb_open(dev);

        l:= usb_get_string_simple(pointerDeviceUSB, dev.descriptor.iManufacturer, buf, 100);
        if l > 0 then
        begin
          Memo.Lines.add('-Manufacturer: '+buf);
          manuf:= buf;
        end;

        l:= usb_get_string_simple(pointerDeviceUSB, dev.descriptor.iProduct, buf, 100);

        if l > 0 then
        begin
          Memo.Lines.add('-Product: '+buf);
          prodStr:= buf;
        end;

        l:= usb_get_string_simple(pointerDeviceUSB, dev.descriptor.iSerialNumber, buf, 100);
        if l > 0 then begin
          Memo.Lines.add('-S/N: '+buf);
          serial:= buf;
        end else
          serial:= 'xx';

        if (prodStr = PRODUCT) and (manuf = MANUFACTURER) then
        begin
          DevObj:= TDevObj.Create();
          DevObj.Serial:= serial;
          DevObj.deviceUSB:= dev;
        end;

        usb_close(pointerDeviceUSB);
      end;

      dev:= dev^.next;
    end;

    bus:= bus^.next;
  end;

  if not ((prodStr = PRODUCT) and (manuf = MANUFACTURER)) then
    statusDevice:= false
  else
    statusDevice:= true;

  Result:= statusDevice;
end;

// Kirim echo
function TForm1.tesEcho(dataInput: Integer): Boolean;
var
  pointerDeviceUSB: pointer;
  bufferBalasan: array[0..2] of byte;
  r: integer;
  nilaiKembalianUSB: integer;
begin
  Result:= false;
  if statusDevice then
  begin
    pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
    if (pointerDeviceUSB <> nil) then
    begin
      Result:= true;

      nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ECHO, dataInput, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
      r:= (bufferBalasan[1] shl 8) or bufferBalasan[0];
      Memo.Lines.Add('Nilai kembalian echo= 0x'+IntToHex(r, 4));

      if r <> dataInput then
        Result:= false;
      if(nilaiKembalianUSB <> 2) then
        Result:= false;

      usb_close(pointerDeviceUSB);
    end;
  end;
end;

procedure TForm1.matikanFungsional;
begin
  Panel4.Enabled:= false;
  Panel4.Visible:= false;
  BtTulisFlash.Enabled:= false;
  BtBacaFlash.Enabled:= false;
  BtReloadFlash.Enabled:= false;
  BtTulisEEP.Enabled:= false;
  BtBacaEEP.Enabled:= false;
  BtReloadEEP.Enabled:= false;
  BtHapusMemori.Enabled:= false;
  MnProgram.Enabled:= false;
end;

procedure TForm1.aktifkanFungsional;
var
  indeksChip: integer;
begin
  Panel4.Enabled:= true;
  Panel4.Visible:= true;
  BtTulisFlash.Enabled:= true;
  BtBacaFlash.Enabled:= true;
  BtReloadFlash.Enabled:= false;
  BtTulisEEP.Enabled:= true;
  BtBacaEEP.Enabled:= true;
  BtReloadEEP.Enabled:= false;
  BtHapusMemori.Enabled:= true;
  MnProgram.Enabled:= true;
  
  fileIni:= TIniFile.Create(ChangeFileExt(Application.ExeName,'.ini'));
  tipeAVR:= fileIni.ReadString('jenis_avr_terpilih','chip','');
  indeksChip:= CmbJenisMK.Items.IndexOf(tipeAVR);
  CmbJenisMK.ItemIndex:= indeksChip;
  
  CmbJenisMKChange(nil);
end;

// Kirim Paket Awal
function TForm1.kirimPaketAwal: Boolean;
var
  pointerDeviceUSB: pointer;
  bufferBalasan: byte;
  bufferKiriman: array[0..2] of byte;
  dataKirimanUSB: integer;
  i, j: integer;
  nilaiKembalianUSB: integer;
  signatureByte: string;
begin
  Result:= false;
  if statusDevice then
  begin
    if (IDPerintah = OPERASI_BACA_FLASH) or (IDPerintah = OPERASI_BACA_EEPROM) or (IDPerintah = OPERASI_BACA_FUSEBITS) then
    begin
		pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
		if (pointerDeviceUSB <> nil) then
		begin
			i:= 1;
			j:= 0;
			while j < 3 do
			begin
				 signatureByte:= Copy(deviceSignature, i, 2);
				 bufferKiriman[j]:= StrToInt('$'+signatureByte);
				 i:= i+3;
				 Inc(j);
			end;
			Memo.Lines.Add('Data Paket Awal= '+deviceSignature);
			nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_KIRIM_PERINTAH_BACA, j, 0, bufferKiriman, j, 5000);
			if nilaiKembalianUSB = j then
			begin
				bufferKiriman[0]:= IDPerintah;// IDPerintah
				bufferKiriman[1]:= 0;
				dataKirimanUSB:= (bufferKiriman[1] shl 8) or bufferKiriman[0];
                                Memo.Lines.Add('Data Paket Awal= '+IntToStr(dataKirimanUSB));
				nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_BACA, dataKirimanUSB, 0, bufferBalasan, 0, 5000);

				statusTransmisi:= true;
				TimerTimeout.Enabled:= true;
				while nilaiKembalianUSB <= 0 do
				begin
					Application.ProcessMessages;
					nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_BACA, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
					if not statusTransmisi then
					begin
						TimerTimeout.Enabled:= false;
						ShowMessage('Transmisi gagal! Re-plug device IRDownloader!');
						break;
					end;
				end;

				if nilaiKembalianUSB = 1 then
				begin
					if bufferBalasan = KIRIM_PAKET_PERINTAH_SELESAI then
					begin
						Result:= true;
						Memo.Lines.Add('Sukses kirim awal');
                                                MemoDataHex.Lines.Clear;
                                                MemoDataASCII.Lines.Clear;
                                                LabelPanjangData.Caption:= '0 bytes';
					end
					else
						Memo.Lines.Add('Gagal kirim awal!');
				end
				else
					Memo.Lines.Add('Gagal kirim awal!');
			end
			else
				Memo.Lines.Add('Gagal kirim awal!');

			usb_close(pointerDeviceUSB);
		end
		else
			Memo.Lines.Add('Gagal kirim awal!');
    end
    else if IDPerintah = OPERASI_HAPUS_CHIP then
    begin
		pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
		if (pointerDeviceUSB <> nil) then
		begin
			bufferKiriman[0]:= IDPerintah;// IDPerintah
			bufferKiriman[1]:= 0;
			dataKirimanUSB:= (bufferKiriman[1] shl 8) or bufferKiriman[0];
			Memo.Lines.Add('Data Paket Awal= '+IntToStr(dataKirimanUSB));
			nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_TULIS, dataKirimanUSB, 0, bufferBalasan, 0, 5000);
			
			statusTransmisi:= true;
			TimerTimeout.Enabled:= true;
			while nilaiKembalianUSB <= 0 do
			begin
				Application.ProcessMessages;
				nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_TULIS, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
				if not statusTransmisi then
				begin
					TimerTimeout.Enabled:= false;
					ShowMessage('Transmisi gagal! Re-plug device IRDownloader!');
					break;
				end;
			end;
			
			if nilaiKembalianUSB = 1 then
			begin
				if bufferBalasan = KIRIM_PAKET_PERINTAH_SELESAI then
				begin
					Result:= true;
					Memo.Lines.Add('Sukses kirim awal');
				end
				else
					Memo.Lines.Add('Gagal kirim awal!');
			end
			else
				Memo.Lines.Add('Gagal kirim awal!');
			
			usb_close(pointerDeviceUSB);
		end
		else
			Memo.Lines.Add('Gagal kirim awal!');
    end
    else
    begin
            if isiHex <> '' then
	    begin
			pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
			if (pointerDeviceUSB <> nil) then
			begin
				if (panjangData div PANJANG_DATA) = 0 then
					jumlahPaket:= 1
				else
				begin
					jumlahPaket:= panjangData div PANJANG_DATA;
					if (panjangData mod PANJANG_DATA) > 0 then
						Inc(jumlahPaket);
				end;

				bufferKiriman[0]:= IDPerintah;// IDPerintah
				bufferKiriman[1]:= jumlahPaket;// Jumlah paket yang akan dikirimkan
				dataKirimanUSB:= (bufferKiriman[1] shl 8) or bufferKiriman[0];
				Memo.Lines.Add('Data Paket Awal= '+IntToStr(dataKirimanUSB));
				nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_TULIS, dataKirimanUSB, 0, bufferBalasan, 0, 5000);

				statusTransmisi:= true;
				TimerTimeout.Enabled:= true;
				while nilaiKembalianUSB <= 0 do
				begin
					Application.ProcessMessages;
					nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_PERINTAH_TULIS, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
					if not statusTransmisi then
					begin
						TimerTimeout.Enabled:= false;
						ShowMessage('Transmisi gagal! Re-plug device IRDownloader!');
						break;
					end;
				end;

				if nilaiKembalianUSB = 1 then
				begin
					if bufferBalasan = KIRIM_PAKET_PERINTAH_SELESAI then
					begin
						Result:= true;
						Memo.Lines.Add('Sukses kirim awal');
					end
					else
						Memo.Lines.Add('Gagal kirim awal!');
				end
				else
					Memo.Lines.Add('Gagal kirim awal!');

				usb_close(pointerDeviceUSB);
			end
			else
				Memo.Lines.Add('Gagal kirim awal!');
	    end
	    else
			ShowMessage('Buffer kiriman kosong! Cek ulang!');
    end;
  end
  else
	ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');

  TimerTimeout.Enabled:= false;
end;

// Kirim Paket Akhir
function TForm1.kirimPaketAkhir: Boolean;
var
  pointerDeviceUSB: pointer;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
begin
  Result:= false;
  if statusDevice then
  begin
    pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
    if (pointerDeviceUSB <> nil) then
    begin
      Memo.Lines.Add('Kirim akhir:');
      nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_AKHIR, 0, 0, bufferBalasan, 0, 5000);

      while nilaiKembalianUSB <= 0 do
      begin
        Application.ProcessMessages;
        nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_KIRIM_AKHIR, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
      end;

      if nilaiKembalianUSB = 1 then
      begin
        if bufferBalasan = KIRIM_AKHIR_SELESAI then
        begin
          Result:= true;
          Memo.Lines.Add('Sukses kirim akhir');
        end
        else
          Memo.Lines.Add('Gagal kirim akhir!');
      end
      else
        Memo.Lines.Add('Gagal kirim akhir!');

      usb_close(pointerDeviceUSB);
    end
    else
      Memo.Lines.Add('Gagal kirim akhir!');
  end
  else
    ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
end;

// Inisialisasi Ulang
function TForm1.inisialisasiUlang: Boolean;
var
  pointerDeviceUSB: pointer;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
begin
  Result:= false;
  if statusDevice then
  begin
    pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
    if (pointerDeviceUSB <> nil) then
    begin
      Memo.Lines.Add('Inisialisasi ulang:');
      nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_INISIALISASI, 0, 0, bufferBalasan, 0, 5000);

      while nilaiKembalianUSB <= 0 do
      begin
        Application.ProcessMessages;
        nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_INISIALISASI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
      end;

      if nilaiKembalianUSB = 1 then
      begin
        if bufferBalasan = IDLE then
        begin
          Result:= true;
          Memo.Lines.Add('Sukses inisialisasi ulang');
        end
        else
          Memo.Lines.Add('Gagal inisialisasi ulang!');
      end
      else
        Memo.Lines.Add('Gagal inisialisasi ulang!');

      usb_close(pointerDeviceUSB);
    end
    else
      Memo.Lines.Add('Gagal inisialisasi ulang!');
  end
  else
    ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
end;

procedure TForm1.BtEchoClick(Sender: TObject);
begin
  if tesEcho(StrToInt('$'+EditEcho.Text)) then
    ShowMessage('Echo sukses')
  else
    ShowMessage('Echo gagal!');
end;

procedure TForm1.BtHapusMemoriClick(Sender: TObject);
begin
  IDPerintah:= OPERASI_HAPUS_CHIP;
  if tesEcho(StrToInt('$8888')) then
  begin
    if kirimPaketAwal then
    begin
      if inisialisasiUlang then
        ShowMessage('Hapus memori berhasil');
    end
    else
      ShowMessage('Device IRDownloader tidak merespons! Re-plug device IRDownloader!');
  end;
end;

procedure TForm1.BtBacaFlashClick(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, k: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  waktu: TDateTime;
  masihTerima: Boolean;
begin
  IDPerintah:= OPERASI_BACA_FLASH;
  if tesEcho(StrToInt('$8888')) then
  begin
    if kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          isiHex:= '';
          Memo.Lines.add('Data hex:');
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

              Memo.Lines.add(IntToStr(nilaiKembalianUSB));
              i:= 0;
              while i < PANJANG_DATA_TRANSFER do
              begin
                k:= (buffer[i] shl 8) or buffer[i+1];
                isiHex:= isiHex+IntToHex(k, 4);
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
          
          if inisialisasiUlang then
            ShowMessage('Proses penerimaan selesai,'#13#10't= '+IntToStr(SecondsBetween(waktu, Now))+' detik');
          
          MemoDataHex.Lines.Add(isiHex);
          LabelPanjangData.Caption:= IntToStr(Length(isiHex) div 2)+' bytes';

          usb_close(pointerDeviceUSB);
        end;
      end
      else
        ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
    end
    else
      ShowMessage('Device IRDownloader tidak merespons! Re-plug device IRDownloader!');
  end;
end;

procedure TForm1.BtTulisFlashClick(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, j, k, m, n: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  dataHex: string;
  waktu: TDateTime;
  jmlPaket256: integer;
  jmlDataSisa: integer;
begin
  IDPerintah:= OPERASI_TULIS_FLASH;
  if tesEcho(StrToInt('$8888')) then
  begin
    if kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          j:= 0;
          k:= 0;
          m:= 0;
          if panjangData < PANJANG_BUFFER_PAKET then// panjangData < 256
          begin
            if panjangData < PANJANG_DATA_TRANSFER then// panjangData < 128
            begin
              while j < panjangData do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                i:= i+2;
              end;
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              if nilaiKembalianUSB = j then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end
            else if panjangData = PANJANG_DATA_TRANSFER then// panjangData = 128
            begin
              while j < panjangData do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                i:= i+2;
              end;
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              if nilaiKembalianUSB = j then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end
            else// panjangData > 128
            begin
              while k < panjangData do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  if dataHex = '' then
                    dataHex:= 'FF';
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;
              if nilaiKembalianUSB = PANJANG_DATA_TRANSFER then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end;
          end
          else if panjangData = PANJANG_BUFFER_PAKET then// panjangData = 256
          begin
            while k < panjangData do
            begin
              while j < PANJANG_DATA_TRANSFER do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                Inc(k);
                i:= i+2;
              end;
              usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              j:= 0;
            end;
            Memo.Lines.add('Buffer telah penuh!');
          end
          else// panjangData > 256
          begin
            jmlPaket256:= panjangData div PANJANG_BUFFER_PAKET;
            jmlDataSisa:= panjangData mod PANJANG_BUFFER_PAKET;

            while m < jmlPaket256 do
            begin
              while k < PANJANG_BUFFER_PAKET do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;
              k:= 0;
              Inc(m);

              if (m < jmlPaket256) and (jmlDataSisa = 0) then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                while nilaiKembalianUSB < 1 do
                begin
                  nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                  Application.ProcessMessages;
                end;
              end
              else if (m <= jmlPaket256) and (jmlDataSisa <> 0) then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                while nilaiKembalianUSB < 1 do
                begin
                  nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                  Application.ProcessMessages;
                end;
              end;
            end;

            j:= 0;
            k:= 0;

            if jmlDataSisa > 0 then
            begin
              while k < jmlDataSisa do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  if dataHex = '' then
                    dataHex:= 'FF';
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                n:= j;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;

              while nilaiKembalianUSB <> n do
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, n, 0, buffer, n, 5000);

              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
            end;
            Memo.Lines.add('Buffer telah penuh!');
          end;

          if kirimPaketAkhir then
          begin
            if inisialisasiUlang then
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
end;

procedure TForm1.BtBacaEEPClick(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, k: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  waktu: TDateTime;
  masihTerima: Boolean;
begin
  IDPerintah:= OPERASI_BACA_EEPROM;
  if tesEcho(StrToInt('$8888')) then
  begin
    if kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          isiHex:= '';
          Memo.Lines.add('Data hex:');
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
			  
              Memo.Lines.add(IntToStr(nilaiKembalianUSB));
              i:= 0;
              while i < PANJANG_DATA_TRANSFER do
              begin
                k:= (buffer[i] shl 8) or buffer[i+1];
                isiHex:= isiHex+IntToHex(k, 4);
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

          if inisialisasiUlang then
            ShowMessage('Proses penerimaan selesai,'#13#10't= '+IntToStr(SecondsBetween(waktu, Now))+' detik');

          MemoDataHex.Lines.Add(isiHex);
          LabelPanjangData.Caption:= IntToStr(Length(isiHex) div 2)+' bytes';

          usb_close(pointerDeviceUSB);
        end;
      end
      else
        ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
    end
    else
      ShowMessage('Device IRDownloader tidak merespons! Re-plug device IRDownloader!');
  end;
end;

procedure TForm1.BtTulisEEPClick(Sender: TObject);
var
  pointerDeviceUSB: pointer;
  i, j, k, m, n: integer;
  buffer: array[0..127] of byte;
  bufferBalasan: byte;
  nilaiKembalianUSB: integer;
  dataHex: string;
  waktu: TDateTime;
  jmlPaket256: integer;
  jmlDataSisa: integer;
begin
  IDPerintah:= OPERASI_TULIS_EEPROM;
  if tesEcho(StrToInt('$8888')) then
  begin
    if kirimPaketAwal then
    begin
      if statusDevice then
      begin
        pointerDeviceUSB:= usb_open(DevObj.deviceUSB);
        if (pointerDeviceUSB <> nil) then
        begin
          waktu:= Now;
          i:= 0;
          j:= 0;
          k:= 0;
          m:= 0;
          if panjangData < PANJANG_BUFFER_PAKET then// panjangData < 256
          begin
            if panjangData < PANJANG_DATA_TRANSFER then// panjangData < 128
            begin
              while j < panjangData do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                i:= i+2;
              end;
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              if nilaiKembalianUSB = j then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end
            else if panjangData = PANJANG_DATA_TRANSFER then// panjangData = 128
            begin
              while j < panjangData do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                i:= i+2;
              end;
              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              if nilaiKembalianUSB = j then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end
            else// panjangData > 128
            begin
              while k < panjangData do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  if dataHex = '' then
                    dataHex:= 'FF';
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;
              if nilaiKembalianUSB = PANJANG_DATA_TRANSFER then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);

                Memo.Lines.add('Buffer telah penuh!');
              end;
            end;
          end
          else if panjangData = PANJANG_BUFFER_PAKET then// panjangData = 256
          begin
            while k < panjangData do
            begin
              while j < PANJANG_DATA_TRANSFER do
              begin
                dataHex:= Copy(isiHex, i+1, 2);
                buffer[j]:= StrToInt('$'+dataHex);
                Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                Inc(j);
                Inc(k);
                i:= i+2;
              end;
              usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
              j:= 0;
            end;
            Memo.Lines.add('Buffer telah penuh!');
          end
          else// panjangData > 256
          begin
            jmlPaket256:= panjangData div PANJANG_BUFFER_PAKET;
            jmlDataSisa:= panjangData mod PANJANG_BUFFER_PAKET;

            while m < jmlPaket256 do
            begin
              while k < PANJANG_BUFFER_PAKET do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;
              k:= 0;
              Inc(m);

              if (m < jmlPaket256) and (jmlDataSisa = 0) then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                while nilaiKembalianUSB < 1 do
                begin
                  nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                  Application.ProcessMessages;
                end;
              end
              else if (m <= jmlPaket256) and (jmlDataSisa <> 0) then
              begin
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                while nilaiKembalianUSB < 1 do
                begin
                  nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_CEK_STATUS_TRANSMISI, 0, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
                  Application.ProcessMessages;
                end;
              end;
            end;

            j:= 0;
            k:= 0;

            if jmlDataSisa > 0 then
            begin
              while k < jmlDataSisa do
              begin
                while j < PANJANG_DATA_TRANSFER do
                begin
                  dataHex:= Copy(isiHex, i+1, 2);
                  if dataHex = '' then
                    dataHex:= 'FF';
                  buffer[j]:= StrToInt('$'+dataHex);
                  Memo.Lines.add(IntToStr(i)+': '+(dataHex));
                  Inc(j);
                  Inc(k);
                  i:= i+2;
                end;
                n:= j;
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, j, 0, buffer, j, 5000);
                j:= 0;
              end;

              while nilaiKembalianUSB <> n do
                nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or PCKeDevais, CMD_ISI_BUFFER, n, 0, buffer, n, 5000);

              nilaiKembalianUSB:= usb_control_msg(pointerDeviceUSB, USB_TYPE_VENDOR or USB_RECIP_DEVICE or DevaisKePC, CMD_ISI_BUFFER, $FF01, 0, bufferBalasan, SizeOf(bufferBalasan), 5000);
            end;
            Memo.Lines.add('Buffer telah penuh!');
          end;

          if kirimPaketAkhir then
          begin
            if inisialisasiUlang then
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
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  if not cekDevice then
  begin
    matikanFungsional;
    PlaySound(PChar('negative.wav'), 0, SND_SYNC);
    ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
  end
  else
  begin
    Beep;
    aktifkanFungsional;
    ShowMessage('Device IRDownloader ready!');
  end;
end;

procedure TForm1.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
  if statusDevice then
  begin
    tipeAVR:= CmbJenisMK.Text;
    fileIni.WriteString('jenis_avr_terpilih','chip',tipeAVR);
    fileIni.Free;
  end;
  CloseAction:= caFree;
end;

procedure TForm1.MnLoadFlashClick(Sender: TObject);
var
  baris: string;
  fileFlash: textfile;
  kodeError: integer;
begin
  if OpenDialogFlash.Execute then
  begin
    panjangData:= 0;
    MemoDataHex.Lines.Clear;
    MemoDataASCII.Lines.Clear;
    AssignFile(fileFlash, OpenDialogFlash.FileName);
    Reset(fileFlash);
    isiHex:= '';
    while not eof(fileFlash) do
    begin
      Readln(fileFlash, baris);
      kodeError:= bacaBaris(baris, bufferHex);
      if kodeError = 0 then
      begin
        MemoDataHex.Lines.Add(DataHex);
        isiHex:= isiHex+DataHex;
        MemoDataASCII.Lines.Add(DataASCII)
      end
      else
      begin
        if kodeError = -1 then
          ShowMessage('Bukan intel hex!')
        else if kodeError = -2 then
          ShowMessage('Record type invalid!')
        else if kodeError = -3 then
          ShowMessage('Checksum error!');

        break;
      end;
    end;
    
    LabelPanjangData.Caption:= IntToStr(panjangData)+' bytes';
    CloseFile(fileFlash);
    
    BtReloadFlash.Enabled:= true;
    BtReloadEEP.Enabled:= false;
  end;
end;

procedure TForm1.MnSaveFlashClick(Sender: TObject);
var
  fileFlash: textfile;
  barisData: string;
  dataPerBaris: string;
  alamatData: integer;
  jmlBaris, jmlDataSisa, i, j: integer;
begin
  if SaveDialogFlash.Execute then
  begin
    AssignFile(fileFlash, SaveDialogFlash.FileName);
    ReWrite(fileFlash);
    panjangData:= Length(isiHex) div 2;
    
    jmlBaris:= panjangData div 16;
    jmlDataSisa:= panjangData mod 16;
    
    i:= 1;
    j:= 0;
    alamatData:= 0;
    while j < jmlBaris do
    begin
      barisData:= Copy(isiHex, i, 32);
      dataPerBaris:= tulisBaris(barisData, IntToHex(alamatData, 4));
      WriteLn(fileFlash, dataPerBaris);
      i:= i+32;
      Inc(j);
      alamatData:= alamatData+16;
    end;
    
    if jmlDataSisa <> 0 then
    begin
      barisData:= Copy(isiHex, i, jmlDataSisa*2);
      dataPerBaris:= tulisBaris(barisData, IntToHex(alamatData, 4));
      WriteLn(fileFlash, dataPerBaris);
    end;
    
    WriteLn(fileFlash, ':00000001FF');

    CloseFile(fileFlash);
  end;
end;

procedure TForm1.BtReloadFlashClick(Sender: TObject);
var
  baris: string;
  fileFlash: textfile;
  kodeError: integer;
begin
  panjangData:= 0;
  MemoDataHex.Lines.Clear;
  MemoDataASCII.Lines.Clear;
  AssignFile(fileFlash, OpenDialogFlash.FileName);
  Reset(fileFlash);
  isiHex:= '';
  while not eof(fileFlash) do
  begin
    Readln(fileFlash, baris);
    kodeError:= bacaBaris(baris, bufferHex);
    if kodeError = 0 then
    begin
      MemoDataHex.Lines.Add(DataHex);
      isiHex:= isiHex+DataHex;
      MemoDataASCII.Lines.Add(DataASCII)
    end
    else
    begin
      if kodeError = -1 then
        ShowMessage('Bukan intel hex!')
      else if kodeError = -2 then
        ShowMessage('Record type invalid!')
      else if kodeError = -3 then
        ShowMessage('Checksum error!');

      break;
    end;
  end;

  LabelPanjangData.Caption:= IntToStr(panjangData)+' bytes';
  CloseFile(fileFlash);
end;

procedure TForm1.MnLoadEEPClick(Sender: TObject);
var
  baris: string;
  fileEEPROM: textfile;
  kodeError: integer;
begin
  if OpenDialogEEPROM.Execute then
  begin
    panjangData:= 0;
    MemoDataHex.Lines.Clear;
    MemoDataASCII.Lines.Clear;
    AssignFile(fileEEPROM, OpenDialogEEPROM.FileName);
    Reset(fileEEPROM);
    isiHex:= '';
    while not eof(fileEEPROM) do
    begin
      Readln(fileEEPROM, baris);
      kodeError:= bacaBaris(baris, bufferHex);
      if kodeError = 0 then
      begin
        MemoDataHex.Lines.Add(DataHex);
        isiHex:= isiHex+DataHex;
        MemoDataASCII.Lines.Add(DataASCII)
      end
      else
      begin
        if kodeError = -1 then
          ShowMessage('Bukan intel hex!')
        else if kodeError = -2 then
          ShowMessage('Record type invalid!')
        else if kodeError = -3 then
          ShowMessage('Checksum error!');

        break;
      end;
    end;

    LabelPanjangData.Caption:= IntToStr(panjangData)+' bytes';
    CloseFile(fileEEPROM);
    
    BtReloadEEP.Enabled:= true;
    BtReloadFlash.Enabled:= false;
  end;
end;

procedure TForm1.MnSaveEEPClick(Sender: TObject);
var
  fileEEPROM: textfile;
  barisData: string;
  dataPerBaris: string;
  alamatData: integer;
  jmlBaris, jmlDataSisa, i, j: integer;
begin
  if SaveDialogEEPROM.Execute then
  begin
    AssignFile(fileEEPROM, SaveDialogEEPROM.FileName);
    ReWrite(fileEEPROM);
    panjangData:= Length(isiHex) div 2;
    
    jmlBaris:= panjangData div 32;
    jmlDataSisa:= panjangData mod 32;
    
    i:= 1;
    j:= 0;
    alamatData:= 0;
    while j < jmlBaris do
    begin
      barisData:= Copy(isiHex, i, 64);
      dataPerBaris:= tulisBaris(barisData, IntToHex(alamatData, 4));
      WriteLn(fileEEPROM, dataPerBaris);
      i:= i+64;
      Inc(j);
      alamatData:= alamatData+32;
    end;
    
    if jmlDataSisa <> 0 then
    begin
      barisData:= Copy(isiHex, i, jmlDataSisa*2);
      dataPerBaris:= tulisBaris(barisData, IntToHex(alamatData, 4));
      WriteLn(fileEEPROM, dataPerBaris);
    end;

    WriteLn(fileEEPROM, ':00000001FF');

    CloseFile(fileEEPROM);
  end;
end;

procedure TForm1.BtReloadEEPClick(Sender: TObject);
var
  baris: string;
  fileEEPROM: textfile;
  kodeError: integer;
begin
  panjangData:= 0;
  MemoDataHex.Lines.Clear;
  MemoDataASCII.Lines.Clear;
  AssignFile(fileEEPROM, OpenDialogEEPROM.FileName);
  Reset(fileEEPROM);
  isiHex:= '';
  while not eof(fileEEPROM) do
  begin
    Readln(fileEEPROM, baris);
    kodeError:= bacaBaris(baris, bufferHex);
    if kodeError = 0 then
    begin
      MemoDataHex.Lines.Add(DataHex);
      isiHex:= isiHex+DataHex;
      MemoDataASCII.Lines.Add(DataASCII)
    end
    else
    begin
      if kodeError = -1 then
        ShowMessage('Bukan intel hex!')
      else if kodeError = -2 then
        ShowMessage('Record type invalid!')
      else if kodeError = -3 then
        ShowMessage('Checksum error!');

      break;
    end;
  end;

  LabelPanjangData.Caption:= IntToStr(panjangData)+' bytes';
  CloseFile(fileEEPROM);
end;

procedure TForm1.LogoObdevClick(Sender: TObject);
begin
  //ShellExecute(Handle, 'open', 'http://www.obdev.at/avrusb/', nil, nil, SW_SHOW);
end;

procedure TForm1.MnCekDeviceClick(Sender: TObject);
begin
  if not cekDevice then
  begin
    matikanFungsional;
    PlaySound(PChar('negative.wav'), 0, SND_SYNC);
    ShowMessage('Device IRDownloader tidak ditemukan! Cek koneksi!');
  end
  else
  begin
    Beep;
    aktifkanFungsional;
    ShowMessage('Device IRDownloader ready!');
  end;
end;

procedure TForm1.MnExitClick(Sender: TObject);
begin
  Close;
end;

procedure TForm1.CmbJenisMKChange(Sender: TObject);
begin
  tipeAVR:= CmbJenisMK.Text;
  deviceSignature:= fileIni.ReadString('jenis_avr',CmbJenisMK.Text,'');
  LabelDeviceSignature.Caption:= deviceSignature;
end;

procedure TForm1.MnFuseBitsClick(Sender: TObject);
var
  frmFuse: TForm2;
begin
  frmFuse:= TForm2.Create(Self);
  frmFuse.ShowModal;
end;

procedure TForm1.TimerTimeoutTimer(Sender: TObject);
begin
  if statusTransmisi then
    statusTransmisi:= false;
end;

initialization
  {$I main.lrs}

end.

