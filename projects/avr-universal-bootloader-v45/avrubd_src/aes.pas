{
  Project:  AVR Universal Bootloader Download
  File:     PC1.pas
  Describe: AES crypt algorithm pascal unit
            base on AVR231's aes code
            128/256 bits key
  Author:   Shaoziyang
  Date:     2008.6
  Version:  1.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit aes;

interface

uses
  SysUtils;

const
  //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
  BPOLY = $1B;

  //!< Block size in number of bytes.
  BLOCKSIZE = 16;

procedure aesMode(key: Integer);
procedure aesKey(key: PByteArray);
procedure aesEncInit;
procedure aesEncrypt(buffer, chainBlock: PByteArray);
procedure aesDecInit;
procedure aesDecrypt(buffer, chainBlock: PByteArray);
procedure aesEncBlock(buffer: PByteArray; key:PByteArray; Len: Integer; mode: Integer=128);
procedure aesDecBlock(buffer: PByteArray; key:PByteArray; Len: Integer; mode: Integer=128);

implementation

var
  kTable: array[0..31] of Byte =
  (
    $D0, $94, $3F, $8C, $29, $76, $15, $D8,
    $20, $40, $E3, $27, $45, $D8, $48, $AD,
    $EA, $8B, $2A, $73, $16, $E9, $B0, $49,
    $45, $B3, $39, $28, $0A, $C3, $28, $3C
    );

  block1: array[0..255] of Byte; //!< Workspace 1.
  block2: array[0..255] of Byte; //!< Worksapce 2.
  tempbuf: array[0..255] of Byte;
  chainCipherBlock: array[0..15] of Byte;

  powTbl: PByteArray; //!< Final location of exponentiation lookup table.
  logTbl: PByteArray; //!< Final location of logarithm lookup table.
  sBox: PByteArray; //!< Final location of s-box.
  sBoxInv: PByteArray; //!< Final location of inverse s-box.
  expandedKey: PByteArray; //!< Final location of expanded key.

  ROUNDS: Byte = 10; //!< Number of rounds.
  KEYLENGTH: Byte = 16; //!< Key length in number of bytes.

procedure aesMode(key: Integer);
begin
  if key <= 128 then
  begin
    ROUNDS := 10;
    KEYLENGTH := 16;
  end
  else
  begin
    ROUNDS := 14;
    KEYLENGTH := 32;
  end;
end;

procedure aesKey(key: PByteArray);
var
  i: Integer;
begin
  for i := 0 to KEYLENGTH - 1 do
    kTable[i] := key^[i];
end;

function CalcDat(t: Byte): Byte;
begin
  if (t and $80) = $80 then
    Result := ((t * 2) xor BPOLY)
  else
    Result := (t * 2);
end;

procedure CalcPowLog(powTbl, logTbl: PByteArray);
var
  i, t: Byte;
begin
  i := 0;
  t := 1;
  repeat
    // Use 0x03 as root for exponentiation and logarithms.
    powTbl^[i] := t;
    logTbl^[t] := i;
    i := i + 1;

    // Muliply t by 3 in GF(2^8).
    t := t xor CalcDat(t);
  until (t = 1); // Cyclic properties ensure that i < 255.

  powTbl^[255] := powTbl^[0]; // 255 = '-0', 254 = -1, etc.
end;

procedure CalcSBox(sBox: PByteArray);
var
  i, rot: Byte;
  temp: Byte;
  Result: Byte;
begin
  // Fill all entries of sBox[].
  i := 0;
  repeat
    //Inverse in GF(2^8).
    if (i > 0) then
    begin
      temp := powTbl^[255 - logTbl^[i]];
    end
    else
    begin
      temp := 0;
    end;

    // Affine transformation in GF(2).
    Result := temp xor $63; // Start with adding a vector in GF(2).
    for rot := 1 to 4 do
    begin
      // Rotate left.
      temp := (temp shl 1) or (temp shr 7);

      // Add rotated byte in GF(2).
      Result := Result xor temp;
    end;

    // Put result in table.
    sBox^[i] := Result;
    i := i + 1;
  until (i = 0);
end;

procedure CalcSBoxInv(sBox, sBoxInv: PByteArray);
var
  i, j: Byte;
begin
  i := 0;
  j := 0;
  // Iterate through all elements in sBoxInv using  i.
  repeat

    // Search through sBox using j.
    repeat
      // Check if current j is the inverse of current i.
      if (sBox^[j] = i) then
      begin
        // If so, set sBoxInc and indicate search finished.
        sBoxInv^[i] := j;
        j := 255;
      end;
      j := j + 1;
    until (j = 0);
    i := i + 1;
  until (i = 0);
end;

procedure CycleLeft(row: PByteArray);
var
  temp: Byte;
begin
  // Cycle 4 bytes in an array left once.
  temp := row^[0];
  row^[0] := row^[1];
  row^[1] := row^[2];
  row^[2] := row^[3];
  row^[3] := temp;
end;

procedure InvMixColumn(column: PByteArray);
var
  r0, r1, r2, r3: Byte;
begin

  r0 := column^[1] xor column^[2] xor column^[3];
  r1 := column^[0] xor column^[2] xor column^[3];
  r2 := column^[0] xor column^[1] xor column^[3];
  r3 := column^[0] xor column^[1] xor column^[2];

  column^[0] := CalcDat(column^[0]);
  column^[1] := CalcDat(column^[1]);
  column^[2] := CalcDat(column^[2]);
  column^[3] := CalcDat(column^[3]);

  r0 := r0 xor column^[0] xor column^[1];
  r1 := r1 xor column^[1] xor column^[2];
  r2 := r2 xor column^[2] xor column^[3];
  r3 := r3 xor column^[0] xor column^[3];

  column^[0] := CalcDat(column^[0]);
  column^[1] := CalcDat(column^[1]);
  column^[2] := CalcDat(column^[2]);
  column^[3] := CalcDat(column^[3]);

  r0 := r0 xor column^[0] xor column^[2];
  r1 := r1 xor column^[1] xor column^[3];
  r2 := r2 xor column^[0] xor column^[2];
  r3 := r3 xor column^[1] xor column^[3];

  column^[0] := CalcDat(column^[0]);
  column^[1] := CalcDat(column^[1]);
  column^[2] := CalcDat(column^[2]);
  column^[3] := CalcDat(column^[3]);

  column^[0] := column^[0] xor column^[1] xor column^[2] xor column^[3];
  r0 := r0 xor column^[0];
  r1 := r1 xor column^[0];
  r2 := r2 xor column^[0];
  r3 := r3 xor column^[0];

  column^[0] := r0;
  column^[1] := r1;
  column^[2] := r2;
  column^[3] := r3;
end;

procedure SubBytes(bytes: PByteArray; count: Byte);
var
  i: Byte;
begin
  i := 0;
  repeat
    bytes^[i] := sBox^[bytes^[i]]; // Substitute every byte in state.
    i := i + 1;
    count := count - 1;
  until (count = 0);
end;

procedure InvSubBytesAndXOR(bytes, key: PByteArray; count: Byte);
var
  i: Byte;
begin
  i := 0;
  repeat
    // *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
    bytes^[i] := block2[bytes^[i]] xor key^[i]; // Use block2 directly. Increases speed.
    i := i + 1;
    count := count - 1;
  until (count = 0);
end;

procedure InvShiftRows(state: PByteArray);
var
  temp: Byte;
begin
  // Note: State is arranged column by column.

  // Cycle second row right one time.
  temp := state^[1 + 3 * 4];
  state^[1 + 3 * 4] := state^[1 + 2 * 4];
  state^[1 + 2 * 4] := state^[1 + 1 * 4];
  state^[1 + 1 * 4] := state^[1 + 0 * 4];
  state^[1 + 0 * 4] := temp;

  // Cycle third row right two times.
  temp := state^[2 + 0 * 4];
  state^[2 + 0 * 4] := state^[2 + 2 * 4];
  state^[2 + 2 * 4] := temp;
  temp := state^[2 + 1 * 4];
  state^[2 + 1 * 4] := state^[2 + 3 * 4];
  state^[2 + 3 * 4] := temp;

  // Cycle fourth row right three times, ie. left once.
  temp := state^[3 + 0 * 4];
  state^[3 + 0 * 4] := state^[3 + 1 * 4];
  state^[3 + 1 * 4] := state^[3 + 2 * 4];
  state^[3 + 2 * 4] := state^[3 + 3 * 4];
  state^[3 + 3 * 4] := temp;
end;

procedure InvMixColumns(state: PByteArray);
begin
  InvMixColumn(@state[0 * 4]);
  InvMixColumn(@state[1 * 4]);
  InvMixColumn(@state[2 * 4]);
  InvMixColumn(@state[3 * 4]);
end;

procedure XORBytes(bytes1, bytes2: PByteArray; count: Byte);
var
  i: Integer;
begin
  i := 0;
  repeat
    bytes1^[i] := bytes1^[i] xor bytes2^[i]; // Add in GF(2), ie. XOR.
    i := i + 1;
    count := count - 1;
  until (count = 0);
end;

procedure CopyBytes(a, b: PByteArray; count: Byte);
var
  i: Byte;
begin
  i := 0;
  repeat
    a^[i] := b^[i];
    i := i + 1;
    count := count - 1;
  until (count = 0);
end;

procedure KeyExpansion(expandedKey: PByteArray);
var
  temp: array[0..3] of Byte;
  i: Byte;
  Rcon: array[0..3] of Byte; // Round constant.
  key: PByte;
begin
  Rcon[0] := 1;
  Rcon[1] := 0;
  Rcon[2] := 0;
  Rcon[3] := 0;

  key := @kTable;

  // Copy key to start of expanded key.
  {i := KEYLENGTH;
  repeat
    expandedKey^[0] := key^;
    inc(PByte(expandedKey), 1);
    inc(key, 1);
    i := i - 1;
  until (i = 0);}
  CopyBytes(expandedKey, PByteArray(key), KEYLENGTH);
  Inc(PByte(expandedKey), KEYLENGTH);

  // Prepare last 4 bytes of key in temp.
  dec(PByte(expandedKey), 4);
  temp[0] := expandedKey^[0];
  temp[1] := expandedKey^[1];
  temp[2] := expandedKey^[2];
  temp[3] := expandedKey^[3];
  Inc(PByte(expandedKey), 4);

  // Expand key.
  i := KEYLENGTH;
  while (i < BLOCKSIZE * (ROUNDS + 1)) do
  begin
    if KEYLENGTH > 24 then
    begin
      // Are we at the start of a multiple of the key size?
      if ((i mod KEYLENGTH) = 0) then
      begin
        CycleLeft(@temp); // Cycle left once.
        SubBytes(@temp, 4); // Substitute each byte.
        XORBytes(@temp, @Rcon, 4); // Add constant in GF(2).
        Rcon[0] := CalcDat(Rcon[0]);

        // Keysize larger than 24 bytes, ie. larger that 192 bits?
      end
        // Are we right past a block size?
      else
        if ((i mod KEYLENGTH) = BLOCKSIZE) then
          SubBytes(@temp, 4); // Substitute each byte.
    end
    else
    begin
      if ((i mod KEYLENGTH) = 0) then
      begin
        CycleLeft(@temp); // Cycle left once.
        SubBytes(@temp, 4); // Substitute each byte.
        XORBytes(@temp, @Rcon, 4); // Add constant in GF(2).
        Rcon[0] := CalcDat(Rcon[0]);
      end;
    end;

    // Add bytes in GF(2) one KEYLENGTH away.
    dec(PByte(expandedKey), KEYLENGTH);
    XORBytes(@temp, expandedKey, 4);
    Inc(PByte(expandedKey), KEYLENGTH);

    // Copy result to current 4 bytes.
    {expandedKey[0] := temp[0];
    expandedKey[1] := temp[1];
    expandedKey[2] := temp[2];
    expandedKey[3] := temp[3];}
    CopyBytes(expandedKey, @temp, 4);
    Inc(PByte(expandedKey), 4);
    i := i + 4; // Next 4 bytes.
  end;
end;

procedure InvCipher(block, expandedKey: PByteArray);
var
  round: Byte;
begin
  round := ROUNDS - 1;
  Inc(PByte(expandedKey), BLOCKSIZE * ROUNDS);

  XORBytes(block, expandedKey, 16);
  dec(PByte(expandedKey), BLOCKSIZE);

  repeat
    InvShiftRows(block);
    InvSubBytesAndXOR(block, expandedKey, 16);
    dec(PByte(expandedKey), BLOCKSIZE);
    InvMixColumns(block);
    round := round - 1;
  until (round = 0);

  InvShiftRows(block);
  InvSubBytesAndXOR(block, expandedKey, 16);
end;

procedure aesDecInit;
var
  i: Integer;
begin
  powTbl := @block1;
  logTbl := @block2;
  CalcPowLog(powTbl, logTbl);

  sBox := @tempbuf;
  CalcSBox(sBox);

  expandedKey := @block1;
  KeyExpansion(expandedKey);

  sBoxInv := @block2; // Must be block2.
  CalcSBoxInv(sBox, sBoxInv);
  for i := 0 to 15 do
    chainCipherBlock[i] := 0;
end;

procedure aesDecrypt(buffer, chainBlock: PByteArray);
var
  temp: array[0..BLOCKSIZE - 1] of Byte;
begin
  CopyBytes(@temp, buffer, BLOCKSIZE);
  InvCipher(buffer, expandedKey);
  XORBytes(buffer, chainBlock, BLOCKSIZE);
  CopyBytes(chainBlock, @temp, BLOCKSIZE);
end;

function Multiply(num, factor: Byte): Byte;
var
  mask: Byte;
begin
  mask := 1;
  Result := 0;
  while (mask <> 0) do
  begin
    // Check bit of factor given by mask.
    if ((mask and factor) <> 0) then
    begin
      // Add current multiple of num in GF(2).
      Result := Result xor num;
    end;

    // Shift mask to indicate next bit.
    mask := mask shl 1;

    // Double num.
    num := CalcDat(num);
  end;
end;

function DotProduct(vector1, vector2: PByteArray): Byte;
begin
  Result := 0;
  Result := Result xor Multiply(vector1^[0], vector2^[0]);
  Inc(PByte(vector1));
  Inc(PByte(vector2));
  Result := Result xor Multiply(vector1^[0], vector2^[0]);
  Inc(PByte(vector1));
  Inc(PByte(vector2));
  Result := Result xor Multiply(vector1^[0], vector2^[0]);
  Inc(PByte(vector1));
  Inc(PByte(vector2));
  Result := Result xor Multiply(vector1^[0], vector2^[0]);
end;

procedure MixColumn(column: PByteArray);
var
  // Prepare first row of matrix twice, to eliminate need for cycling.
  row: array[0..7] of Byte;
  Result: array[0..3] of Byte;
begin
  row[0] := $02;
  row[1] := $03;
  row[2] := $01;
  row[3] := $01;
  row[4] := $02;
  row[5] := $03;
  row[6] := $01;
  row[7] := $01;

  // Take dot products of each matrix row and the column vector.
  Result[0] := DotProduct(@row[0], column);
  Result[1] := DotProduct(@row[3], column);
  Result[2] := DotProduct(@row[2], column);
  Result[3] := DotProduct(@row[1], column);

  // Copy temporary result to original column.
  column^[0] := Result[0];
  column^[1] := Result[1];
  column^[2] := Result[2];
  column^[3] := Result[3];
end;

procedure MixColumns(state: PByteArray);
begin
  MixColumn(@state[0 * 4]);
  MixColumn(@state[1 * 4]);
  MixColumn(@state[2 * 4]);
  MixColumn(@state[3 * 4]);
end;

procedure ShiftRows(state: PByteArray);
var
  temp: Byte;
begin
  // Note: State is arranged column by column.

  // Cycle second row left one time.
  temp := state^[1 + 0 * 4];
  state^[1 + 0 * 4] := state^[1 + 1 * 4];
  state^[1 + 1 * 4] := state^[1 + 2 * 4];
  state^[1 + 2 * 4] := state^[1 + 3 * 4];
  state^[1 + 3 * 4] := temp;

  // Cycle third row left two times.
  temp := state^[2 + 0 * 4];
  state^[2 + 0 * 4] := state^[2 + 2 * 4];
  state^[2 + 2 * 4] := temp;
  temp := state^[2 + 1 * 4];
  state^[2 + 1 * 4] := state^[2 + 3 * 4];
  state^[2 + 3 * 4] := temp;

  // Cycle fourth row left three times, ie. right once.
  temp := state^[3 + 3 * 4];
  state^[3 + 3 * 4] := state^[3 + 2 * 4];
  state^[3 + 2 * 4] := state^[3 + 1 * 4];
  state^[3 + 1 * 4] := state^[3 + 0 * 4];
  state^[3 + 0 * 4] := temp;
end;

procedure Cipher(block, expandedKey: PByteArray);
var
  round: Byte;
begin
  round := ROUNDS - 1;
  XORBytes(block, expandedKey, 16);
  Inc(PByte(expandedKey), BLOCKSIZE);

  repeat
    SubBytes(block, 16);
    ShiftRows(block);
    MixColumns(block);
    XORBytes(block, expandedKey, 16);
    Inc(PByte(expandedKey), BLOCKSIZE);
    round := round - 1;
  until (round = 0);

  SubBytes(block, 16);
  ShiftRows(block);
  XORBytes(block, expandedKey, 16);
end;

procedure aesEncInit;
var
  i: Integer;
begin
  powTbl := @block1;
  logTbl := @tempbuf;
  CalcPowLog(powTbl, logTbl);

  sBox := @block2;
  CalcSBox(sBox);

  expandedKey := @block1;
  KeyExpansion(expandedKey);

  for i := 0 to 15 do
    chainCipherBlock[i] := 0;
end;

procedure aesEncrypt(buffer, chainBlock: PByteArray);
begin
  XORBytes(buffer, chainBlock, BLOCKSIZE);
  Cipher(buffer, expandedKey);
  CopyBytes(chainBlock, buffer, BLOCKSIZE);
end;

procedure aesEncBlock(buffer: PByteArray; key:PByteArray; Len: Integer; mode: Integer);
var
  i: Integer;
begin
  aesMode(mode);
  aesKey(key);
  aesEncInit;

  i := 0;
  while i <= (Len - 16) do
  begin
    aesEncrypt(@buffer^[i], @chainCipherBlock);
    i := i + 16;
  end;
end;

procedure aesDecBlock(buffer: PByteArray; key:PByteArray; Len: Integer; mode: Integer);
var
  i: Integer;
begin
  aesMode(mode);
  aesKey(key);
  aesDecInit;

  i := 0;
  while i <= (Len - 16) do
  begin
    aesDecrypt(@buffer^[i], @chainCipherBlock);
    i := i + 16;
  end;
end;

initialization
  aesMode(128);
finalization

end.

