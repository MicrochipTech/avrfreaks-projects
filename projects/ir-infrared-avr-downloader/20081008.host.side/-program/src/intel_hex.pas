unit intel_hex; 

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}
{$H+}

interface

uses
  Classes, SysUtils;

type
  buffer= packed array of byte;

var
  bufferHex: buffer;
  isiHex: string;
  Data: byte;
  DataHex: string;
  DataASCII: string;
  panjangData: integer;

function bacaBaris(HexLine: string; var Buf: buffer): integer;
function tulisBaris(HexLine: string; alamatData: string): string;

implementation

// Parsing file Intel Hex
function bacaBaris(HexLine: string; var Buf: buffer): integer;
var
  alamat, count: integer;
  ChkSum, JmlBaris, RecLen, RecType: byte;
  t: string;
begin
  // Atur panjang array, dapat disesuaikan sesuai keinginan
  SetLength(bufferHex, 100000);
  
  if HexLine[1] = ':' then
  begin
    t:= '$'+copy(HexLine, 2, 2);

    // Baca panjang
    RecLen:= StrToInt(t);
    ChkSum:= 0;
    ChkSum:= ChkSum+RecLen;
    t:= '$'+copy(HexLine, 4, 4);

    // Baca alamat
    alamat:= StrToInt(t);
    ChkSum:= ChkSum+lo(alamat)+hi(alamat);
    t:= '$'+copy(HexLine, 8, 2);
    RecType:= StrToInt(t);
    ChkSum:= ChkSum+RecType;
    DataHex:= '';
    DataASCII:= '';

    case RecType of
      0:
      begin
        // Blok data
        count:= 0;
        while(count < RecLen) do
        begin
          t:= '$'+copy(HexLine, 10+2*count, 2);
          DataASCII:= DataASCII+Chr(StrToInt(t));
          DataHex:= DataHex+copy(HexLine, 10+2*count, 2);
          Data:= StrToInt(t);
          ChkSum:= ChkSum+Data;
          Buf[alamat+count]:= Data;
          Inc(count);
          Inc(panjangData);
        end;
        t:= '$'+copy(HexLine, 10+2*count, 2);
        JmlBaris:= StrToInt(t);
        result:= 0;
      end;
      1:
      begin
        // End of file
        t:= '$'+copy(HexLine, 10, 2);
        JmlBaris:= StrToInt(t);
        result:= 1;
      end;
      else
      begin
        // Record type salah
        result:= -2;
        exit;
      end;
      
      Data:= JmlBaris+ChkSum;
      if Data <> 0 then
        result:= -3
      else
        result:= -1;
    end;
  end;
end;

// Bangun file Intel Hex
function tulisBaris(HexLine: string; alamatData: string): string;
var
  jmlDataPerBaris, i: integer;
  isiPerBaris: string;
  nilaiChecksum: byte;
begin
  jmlDataPerBaris:= Length(HexLine) div 2;
  
  isiPerBaris:= IntToHex(jmlDataPerBaris, 2)+alamatData+'00'+HexLine;
  i:= 1;
  nilaiChecksum:= 0;
  while i <= Length(isiPerBaris) do
  begin
    nilaiChecksum:= nilaiChecksum+StrToInt('$'+Copy(isiPerBaris, i, 2));
    i:= i+2;
  end;
  nilaiChecksum:= 256-nilaiChecksum;
  result:= ':'+isiPerBaris+IntToHex(nilaiChecksum, 2);
end;

end.
