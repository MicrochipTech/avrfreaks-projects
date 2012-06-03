{
  Project:  AVR Universal Bootloader Download
  File:     PC1.pas
  Describe: PC1 Cipher Algorithm(Pukall Cipher 1)
            form "http://membres.lycos.fr/pc1/"
            128/256 bits key
  Author:   Shaoziyang
  Date:     2008.4
  Version:  1.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit PC1;

interface

var
  ax, bx, cx, dx, si, x1a2, inter, cfc, cfd: word;
  x1a0: array[0..16] of word;
  cle: array[0..32] of byte;

procedure PC1Encrypt128(buf: array of byte; var cbuf: array of byte; size: Integer);
procedure PC1Decrypt128(buf: array of byte; var cbuf: array of byte; size: Integer);
procedure PC1DeleteKey128;
procedure PC1SetKey128(key: array of byte);
function PC1Cod128Byte(c: byte): byte;
function PC1Dec128Byte(c: byte): byte;

procedure PC1Encrypt256(buf: array of byte; var cbuf: array of byte; size: Integer);
procedure PC1Decrypt256(buf: array of byte; var cbuf: array of byte; size: Integer);
procedure PC1DeleteKey256;
procedure PC1SetKey256(key: array of byte);
function PC1Cod256Byte(c: byte): byte;
function PC1Dec256Byte(c: byte): byte;

implementation

procedure exchange(var a, b: word);
var
  tmp: Word;
begin
  tmp := a;
  a := b;
  b := tmp;
end;

procedure PC1Decrypt128(buf: array of byte; var cbuf: array of byte; size: Integer);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to size - 1 do
    cbuf[n] := PC1Dec128Byte(buf[n]);
end;

procedure PC1Encrypt128(buf: array of byte; var cbuf: array of byte; size: Integer);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to size - 1 do
    cbuf[n] := PC1Cod128Byte(buf[n]);
end;

procedure PC1DeleteKey128;
var
  n: Integer;
begin
  for n := 0 to 15 do
    cle[n] := 0;
  ax := 0;
  bx := 0;
  cx := 0;
  dx := 0;
  si := 0;
  x1a2 := 0;
  x1a0[0] := 0;
  x1a0[1] := 0;
  x1a0[2] := 0;
  x1a0[3] := 0;
  x1a0[4] := 0;
  inter := 0;
  cfc := 0;
  cfd := 0;
end;

procedure PC1SetKey128(key: array of byte);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to 15 do
    cle[n] := key[n];
end;

procedure PC1assemble128;
var
  i: Integer;
begin
  inter := 0;
  x1a0[0] := 0;
  for i := 0 to 7 do
  begin

    x1a0[i + 1] := x1a0[i] xor ((cle[i * 2] * 256) + cle[i * 2 + 1]);

    dx := x1a2 + i;
    ax := x1a0[i + 1];
    cx := $015A;
    bx := $4E35;

    exchange(ax, si);
    exchange(ax, dx);

    if (ax <> 0) then
    begin
      ax := ax * bx;
    end;
    exchange(ax, cx);

    if (ax <> 0) then
    begin
      ax := ax * si;
      cx := ax + cx;
    end;
    exchange(ax, si);

    ax := ax * bx;
    dx := cx + dx;

    ax := ax + 1;

    x1a2 := dx;
    x1a0[i + 1] := ax;

    inter := inter xor (ax xor dx);
  end;
end;

function PC1Cod128Byte(c: byte): byte;
var
  i: Integer;
begin
  PC1assemble128;
  cfc := inter shr 8;
  cfd := inter and 255;
  for i := 0 to 15 do
    cle[i] := cle[i] xor c;

  result := c xor (cfc xor cfd);
end;

function PC1Dec128Byte(c: byte): byte;
var
  i: Integer;
begin
  PC1assemble128;
  cfc := inter shr 8;
  cfd := inter and 255;
  result := c xor (cfc xor cfd);
  for i := 0 to 15 do
    cle[i] := cle[i] xor Result;
end;

procedure PC1assemble256;
var
  i: Integer;
begin
  inter := 0;
  x1a0[0] := 0;
  for i := 0 to 15 do
  begin

    x1a0[i + 1] := x1a0[i] xor ((cle[i * 2] * 256) + cle[i * 2 + 1]);

    dx := x1a2 + i;
    ax := x1a0[i + 1];
    cx := $015A;
    bx := $4E35;

    exchange(ax, si);
    exchange(ax, dx);

    if (ax <> 0) then
    begin
      ax := ax * bx;
    end;
    exchange(ax, cx);

    if (ax <> 0) then
    begin
      ax := ax * si;
      cx := ax + cx;
    end;
    exchange(ax, si);

    ax := ax * bx;
    dx := cx + dx;

    ax := ax + 1;

    x1a2 := dx;
    x1a0[i + 1] := ax;

    inter := inter xor (ax xor dx);
  end;
end;

procedure PC1DeleteKey256;
var
  n: Integer;
begin
  for n := 0 to 31 do
    cle[n] := 0;
  ax := 0;
  bx := 0;
  cx := 0;
  dx := 0;
  si := 0;
  x1a2 := 0;
  x1a0[0] := 0;
  x1a0[1] := 0;
  x1a0[2] := 0;
  x1a0[3] := 0;
  x1a0[4] := 0;
  inter := 0;
  cfc := 0;
  cfd := 0;
end;

procedure PC1SetKey256(key: array of byte);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to 31 do
    cle[n] := key[n];
end;

procedure PC1Decrypt256(buf: array of byte; var cbuf: array of byte; size: Integer);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to size - 1 do
    cbuf[n] := PC1Dec256Byte(buf[n]);
end;

procedure PC1Encrypt256(buf: array of byte; var cbuf: array of byte; size: Integer);
var
  n: Integer;
begin
  si := 0;
  x1a2 := 0;
  for n := 0 to size - 1 do
    cbuf[n] := PC1Cod256Byte(buf[n]);
end;

function PC1Cod256Byte(c: byte): byte;
var
  i: Integer;
begin
  PC1assemble256;
  cfc := inter shr 8;
  cfd := inter and 255;
  for i := 0 to 31 do
    cle[i] := cle[i] xor c;

  result := c xor (cfc xor cfd);
end;

function PC1Dec256Byte(c: byte): byte;
var
  i: Integer;
begin
  PC1assemble256;
  cfc := inter shr 8;
  cfd := inter and 255;
  result := c xor (cfc xor cfd);
  for i := 0 to 31 do
    cle[i] := cle[i] xor Result;
end;

end.

