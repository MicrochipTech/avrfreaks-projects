Attribute VB_Name = "modWIZ_001153"
Option Explicit

Public Declare Function GetPrivateProfileString& Lib "Kernel32" Alias _
  "GetPrivateProfileStringA" (ByVal lpAppName As String, ByVal lpKeyName _
  As String, ByVal lpDefault As String, ByVal lpRtnStr As String, ByVal _
  nSize As Long, ByVal lpFName As String)
Public Declare Function WritePrivateProfileString& Lib "Kernel32" Alias _
  "WritePrivateProfileStringA" (ByVal lpAppName As String, ByVal _
  lpKeyName As String, ByVal lpString As String, ByVal lpFName As String)
  
Type pps                        ' private profile string
  sect As String                ' section
  key As String                 ' key
  s As String                   ' original value
  tb As Object                  ' object
End Type

Type cust                       ' customization definition
  lnum As Integer               ' line number in file
  typ As Integer                ' 0 = static, 1 = string, 2 = number
  addr As Integer               ' base address
  nb As Integer                 ' number of bytes
  nxt As Long                   ' next value (if typ = 2)
  incr As Long                  ' increment (if typ = 2)
  sval As String                ' values to patch
End Type

Global Const ACK = "A"
Global Const S_DEAD = 0               ' haven't started yet
Global Const S_LISTEN = 1             ' listening
Global Const S_CONNECT1 = 2           ' connected, waiting ACK
Global Const S_CONNECT2 = 3           ' connected
Global Const S_READ_E = 4             ' reading EEPROM
Global Const S_READ_F = 5             ' reading flash
Global Const S_WRITE_E = 6            ' writing EEPROM
Global Const S_WRITE_F = 7            ' writing flash
Global Const S_VERIFY_E = 8           ' verifying EEPROM
Global Const S_VERIFY_F = 9           ' verifying flash
Global Const S_JUMP = 10              ' jumping to application
Global Const S_ERASE = 11             ' erasing flash

Global Const FLASHSIZE = 14336        ' bytes of flash in ATmega169
Global Const FPAGES = 112             ' number of flash pages
Global Const FPAGESIZE = 128          ' bytes per flash page
Global Const EPAGESIZE = 128          ' bytes per EEPROM page (our def)
Global Const EPAGES = 4               ' number of EEPROM pages
Global Const EEPROMSIZE = 512         ' bytes of eeprom in ATmega169
Global Const PMAX = 40                ' max numbers in parse()

Global OnLine As Boolean              ' whether we're alive or not
Global fl(FLASHSIZE - 1) As Integer   ' flash memory image
Global ee(EEPROMSIZE - 1) As Integer  ' eeprom memory image
Global ini_file$                      ' location and name of the ini file
Global internal As Boolean            ' whether we're doing something
Global hold(5) As pps                 ' .ini file strings, original
Global EditChg As Integer             ' what the editor said (-1, 0, or 1)
Global EditTop As Integer             ' top line of image to/from editor
Global patch() As cust                ' patches
Global npatch As Integer              ' number of patches
Global nDim As Integer                ' dimensioned size of patch()
Global nparse As Integer              ' number of parsed elements
Global parse(PMAX) As Long            ' parsed elements
Global rFlag As Integer               ' TCP receiver request/flag
Global Auto As Boolean                ' auto/manual mode
Global TCPstatus As Integer           ' what's going on
Global Buf(FPAGESIZE - 1) As Integer  ' TCP incoming buffer
Global nBuf As Integer                ' number of bytes in nBuf()
Global NxtState$                      ' automated sequence of events
Global GoWith$                        ' autoload's activities, abbreviated
Global rPage As Integer               ' active page
Global eA1 As Integer                 ' active EEPROM lower address
Global eA2 As Integer                 ' active EEPROM upper address
Global fWrite As Boolean              ' whether we're sending undef pages
Global verify As Boolean              ' whether read = verify
Global vOK As Boolean                 ' verify OK
Global eCnt As Integer                ' avoid endless errors

'Sub main()
'  Dim recurse As Boolean
'
'  frmWIZ_001153.Show                  ' let them play
'  DataIn = False
'  Do While True
'    DoEvents
'    If DataIn Then
'      DataIn = False
'      recurse = True
'      Do While recurse
'        Call frmWIZ_001153.StepState(recurse)
'      Loop
'    End If
'  Loop
'
'End Sub

Sub Find_Range(a1 As Integer, a2 As Integer)
' find a range (max len = 128) of bytes in the eeprom image
' which are defined.
' initially, set a1 = 0
' call until a1 is returned < 0. otherwise ee[a1-a2] is good data.
  Dim i As Integer, found As Integer, low As Integer
  
  If a1 < 0 Then a1 = 0
  If a1 >= EEPROMSIZE Then
    a1 = -1
    Exit Sub
  End If
  
  found = 0                       ' how many in sequence
  For i = a1 To EEPROMSIZE - 1
    If ee(i) >= 0 Then
      found = found + 1           ' count that dog
      If found = 1 Then low = i   ' flag the first one
      If found >= 128 Then        ' more is too much
        a1 = low                  ' so go on home
        a2 = i
        Exit Sub
      End If
    Else
      If found > 0 Then           ' we're in a streak
        a1 = low
        a2 = i - 1                ' but it ended
        Exit Sub
      End If
    End If
    Next i
    
    If found = 0 Then
      a1 = -1                     ' didn't see anything
    Else
      a1 = low                    ' good to the very last byte
      a2 = EEPROMSIZE - 1
    End If

End Sub

Sub Next_Page(p As Integer)
' find the next page of fl() with defined data
'
' to use, set p = 0 initially. this returns p in the range of
' 0 - (FPAGES - 1) for data, or -1 if none

  Dim i As Integer, j As Integer, ba As Integer
  
  If p < 0 Then p = 0
  For i = p To FPAGES - 1         ' look at remaining pages
    ba = i * FPAGESIZE            ' base address, this page
    For j = 0 To FPAGESIZE - 1
      If fl(ba + j) >= 0 Then
        p = i                     ' found one
        Exit Sub
      End If
      Next j
    Next i
  p = -1                          ' didn't see any data
    
End Sub

Sub PatchPatchFile(f$)
' apply the updates to the patch file
  Dim linenum As Integer, a$, target As Integer, nxt As Integer
  Dim vmax As Long, t$
  
  If npatch = 0 Then Exit Sub
  
  t$ = Replace$(LCase$(f$), ".txt", ".bak")
  If Dir$(t$) <> "" Then Kill t$
  
  FileCopy f$, t$                       ' make a source copy
  Open t$ For Input As #1               ' old is input
  Open f$ For Output As #2              ' original is output
  linenum = 0                           ' count the lines
  nxt = 1                               ' active patch
  target = patch(nxt).lnum              ' target line number
  
  Do While Not EOF(1)
    Line Input #1, a$
    linenum = linenum + 1
    If linenum = target Then
      With patch(nxt)
        If .typ = 1 Then                ' it's a string
          Mid$(a$, 1, 1) = "*"
        ElseIf .typ = 2 Then            ' it's a number
          If .nb = 4 Then
            vmax = 99999999
          Else
            vmax = 256 ^ .nb - 1
          End If
          If .nxt > vmax Then .nxt = vmax
          a$ = Left$(a$, 7) & .nb & "," & .nxt & "," & .incr
        End If
      End With
      nxt = nxt + 1
      If nxt > npatch Then
        target = 0                      ' not looking any more
      Else
        target = patch(nxt).lnum        ' next line number
      End If
    End If
    Print #2, a$
  Loop
  Close

End Sub

Sub ReadPatchFile(f$, w As Integer)
' grab what we're interested in off the patch file
' f$ - file name
' w - 0 for flash, 1 for eeprom
' we'll let the caller's error handling do the dirty work
'
' patch file format (text file):
'   unchanging values:
'     SE0000 0x2a,123,0     patch eeprom at 0000,0001,0002 with values
'     SF0000                patch flash at 0000 with following string
'     >string               patch to apply (includes any blanks present)
'
'   changing values:
'     CE0010 2,123,12       patch eeprom, 2 bytes, next value 123, incr 12
'     CF0010                patch flash with strings from following lines
'     *this one is done     patch already applied (> becomes *)
'     >next one to do       current patch to be applied
'     >following            patch after current
'
'   byteorder=12,123,1234
'
' if updates are to be applied, the changing value definitions will be
' modified. patches are applied in the order they appear in the file.
' errors are pretty much ignored, so beware.
'
  Dim a$, want$, j As Integer, look As Integer
  Dim linenum As Integer, byteorder$
  
  ReDim patch(25)
  nDim = 25
  npatch = 0
  If w = 0 Then want$ = "f" Else want$ = "e"
  byteorder$ = "12,123,1234"        ' default
  
  look = 0                          ' watching for C or S only
  linenum = 0
  Open f$ For Input As #1
  
  Do While Not EOF(1)
    Line Input #1, a$
    linenum = linenum + 1
    
    j = GoodLine(a$, look, want$)
    If j = 1 Then
      look = 0                          ' look for C/S
      npatch = npatch - 1               ' toss that entry
      j = GoodLine(a$, look, want$)     ' re-interpret this one
    End If
    If j = 0 Then Call CheckBOrder(a$, byteorder$)
    
    If j > 1 Then                       ' good line of some sort
      If j < 6 Then
        npatch = npatch + 1             ' it's a C/S line
        If npatch > nDim Then
          nDim = nDim + 25
          ReDim Preserve patch(nDim)
        End If
        With patch(npatch)
          .lnum = linenum
          .addr = Val("&h" & Mid$(a$, 3, 4))
        End With
      End If

      With patch(npatch)
        Select Case j
          Case 2                 ' ---- short C
            .typ = 1             ' changing string
            look = 1             ' look for string
          Case 3                 ' ---- long C
            .typ = 2             ' changing number
            If cNum(a$, byteorder$) Then     ' decode the scheme
              look = 0           ' not needed, but...
              npatch = npatch - 1  ' error: toss this one out
            End If
          Case 4                 ' ---- short S
            .typ = 0             ' static string
            look = 1             ' look for string
          Case 5                 ' ---- long S
            .typ = 0             ' static number(s)
            If sNum(a$) Then     ' go decode the numbers
              look = 0           ' not needed, but...
              npatch = npatch - 1  ' error: toss this one out
            End If
          Case 6                 ' ---- >
            .lnum = linenum
            .sval = Mid$(a$, 2)
            .nb = Len(.sval)
            look = 0             ' switch back to C/S
        End Select
      End With
    End If
  Loop
  Close

End Sub

Sub CheckBOrder(a$, bo$)
' check for a byteorder command
' a$ - incoming line off of file
' bo$ - returned byte order def, if it is one
' line must be "byteorder=##,###,####" exactly (upper or lower)

  If Len(a$) <> 21 Then Exit Sub
  If LCase(Left$(a$, 10)) <> "byteorder=" Then Exit Sub
  If Mid$(a$, 13, 1) <> "," Then Exit Sub
  If Mid$(a$, 16, 1) <> "," Then Exit Sub
  bo$ = Mid$(a$, 11)

End Sub

Function cNum(a$, ord$) As Boolean
' decode the changing number scheme
' return: True if error, False if OK
' a$ = CE0010 2,123,12      2 bytes, current value 123, incr 12
' ord$ = 12,123,1234 (or other byte order specification)
'   max number of bytes is 4 (7 hex digits, or 99,999,999)
' update patch(npatch):
'   .sval = value, .nb = #bytes, .nxt = new value, .incr = inc
  Dim b$, i As Integer, cptr As Integer, cc$, j As Integer
  
  cc$ = "1," & ord$                ' 1,12,123,1234 or some such
  cNum = True
  b$ = Mid$(a$, 8)
  Call ParseIt(b$)
  If nparse <> 3 Then Exit Function
  
  With patch(npatch)
    If parse(1) > 4 Then Exit Function
    .nb = parse(1)                 ' number of bytes
    cptr = Val(Mid$("0259", .nb, 1)) + 1
    
    .incr = parse(3)               ' increment
    b$ = String$(8, "0") & Hex$(parse(2))   ' current value, padded
    .nxt = parse(2) + .incr        ' what's up for next time
    .sval = String$(.nb, 0)        ' build the value here
    For i = .nb To 1 Step -1
      j = Val(Mid$(cc$, cptr, 1))
      cptr = cptr + 1
      If j < 1 Or j > .nb Then j = 1
      Mid$(.sval, j, 1) = Chr$(Val("&h" & Right$(b$, 2)))
      b$ = Left$(b$, Len(b$) - 2)
      Next i
  End With
  cNum = False
  
End Function

Function sNum(a$) As Boolean
' decode the static number scheme
' return: True if error, False if OK
' SE0000 0x2a,123,0     values in hex or decimal
' update patch(npatch):
'   .sval = values, .nb = #bytes
  Dim b$, i As Integer
  
  sNum = True
  b$ = Mid$(a$, 8)
  Call ParseIt(b$)
  If nparse < 1 Then Exit Function
  
  With patch(npatch)
    .nb = nparse
    .sval = String$(.nb, 0)
    For i = 1 To nparse
      If parse(i) > 255 Then Exit Function
      Mid$(.sval, i, 1) = Chr$(parse(i))
      Next i
  End With
  sNum = False
  
End Function

Sub ParseIt(a$)
' do a comma separated list of hex or decimal numbers
' PMAX is the limit on the number of numbers
  Dim j As Integer, b$, e As Boolean, c$
  
  nparse = 0
  e = False                            ' error flag
  b$ = Trim(a$)
  If b$ = "" Then Exit Sub
  If Right$(b$, 1) <> "," Then b$ = b$ & ","

  Do While b$ <> ""
    j = InStr(b$, ",")
    If j = 1 Then
      b$ = Mid$(b$, 2)                ' ignore double commas
    Else
      c$ = Left$(b$, j - 1)           ' the value
      b$ = Mid$(b$, j + 1)            ' the remainder, if any
      Call Numba(c$, e)               ' convert and put away if good
    End If
  Loop
  If e Then nparse = 0                ' errors ruin our day
  
End Sub

Sub Numba(a$, e As Boolean)
' see if the number in a$ is legal. if so, store in parse(nparse + 1)
' for errors, set e true
' format: decimal or 0x### for hex. max 99,999,999 or 0x5f5e0ff
  Dim n As Integer, b$, c$, i As Integer
  
  b$ = a$
  If Len(b$) > 2 Then
    c$ = LCase$(Left$(b$, 2))
    If c$ = "0x" Then
      b$ = Mid$(b$, 3)
      If Not IsHex(b$) Then
        e = True
        Exit Sub
      End If
      Do While Left$(b$, 1) = "0"
        b$ = Mid$(b$, 2)
      Loop
      If b$ = "" Then b$ = "0"
      If Len(b$) > 7 Then
        e = True
        Exit Sub
      End If
      b$ = CStr(Val("&h" & b$))    ' yeah, it's ugly
    End If
  End If

  n = Len(b$)
  If n > 8 Then
    e = True
    Exit Sub
  End If
  For i = 1 To n
    c$ = Mid$(b$, i, 1)
    If c$ < "0" Or c$ > "9" Then
      e = True
      Exit Sub
    End If
    Next i
  
  If nparse < PMAX Then
    nparse = nparse + 1
  Else
    e = True
  End If
  parse(nparse) = Val(b$)

End Sub

Function GoodLine(r$, look As Integer, want$) As Integer
' see if a line of patch file is what we're looking for
' r$ - incoming line
' look - 0 if we want only C/S, 1 if we want >
' want$ - "e" or "f"
' returns: 0 if bad
'          1 if looking for > and found C or S
'          2,3 for short,long C
'          4,5 for short,long S
'          6 for >
' addresses will be verified as 4 digit hex first digit 0 or 1
  Dim n As Integer, j As Integer, a$
  
  GoodLine = 0                    ' assume bad
  a$ = RTrim$(r$)                 ' ignore trailing blanks
  n = Len(a$)
  If look = 0 Then                ' looking for C/S
    If n < 6 Then Exit Function
    j = InStr("CScs", Left$(a$, 1))
    If j = 0 Then Exit Function
    If j > 2 Then j = j - 2
    If LCase$(Mid$(a$, 2, 1)) <> want$ Then Exit Function
    If Not IsHex(Mid$(a$, 3, 4)) Then Exit Function
    If Mid$(a$, 3, 1) > "1" Then Exit Function  ' addresses 0... & 1... ok
    If n = 6 Then
      GoodLine = j + j            ' short form
    Else
      If n < 8 Then Exit Function
      If Mid$(a$, 7, 1) <> " " Then Exit Function
      GoodLine = j + j + 1        ' long form
    End If
  Else                            ' looking for >
    If n < 2 Then Exit Function
    If Left$(a$, 1) = ">" Then
      GoodLine = 6
    Else
      If n > 5 And InStr("CcSs", Left$(a$, 1)) > 0 Then GoodLine = 1
    End If
  End If

End Function

Sub MergeAscii(w As Integer, aTop As Integer)
' merge an ascii dump file into the fl (w = 0) or ee (w = 1) array
' aTop - maximum legal address
'
' format is AAAA) VV VV VV VV VV VV VV VV  ........
' undefined values are written as "--"
' file is open (#1) on call; leave open
  Dim a$, num As Integer, i As Integer, addr As Integer
  Dim v(8) As Integer, aa As Integer

  Do While Not EOF(1)
    Line Input #1, a$                 ' a line of dump
    If Len(a$) > 7 Then               ' needs at least one value
      Call Decode(a$, aTop, addr, num, v)  ' parse it
      If num > 0 Then                 ' number of good values
        For i = 1 To num
          If v(i) >= 0 Then           ' -1 means undefined
            aa = addr + i - 1         ' the target address
            If w = 0 Then
              fl(aa) = v(i)           ' flash
            Else
              ee(aa) = v(i)           ' eeprom
            End If
          End If
          Next i
      End If
    End If
  Loop

End Sub

Sub Decode(a$, aTop As Integer, addr As Integer, num As Integer, _
  v() As Integer)
' decode a line of ascii dump file:
'   format is AAAA) VV VV VV VV VV VV VV VV  ........
'   undefined values are written as "--"
' max address is aTop
' returns: addr = base address
'          num = # elements (0 - 8)
'          v() = num values starting with (1), if any
  Dim n As Integer, i As Integer, b$, c$, aval As Integer
  
  num = 0                         ' assume bad
  If Mid$(a$, 5, 1) <> ")" Then Exit Sub
  b$ = LCase$(a$)
  n = Len(b$)
  If n > 29 Then n = 29
  
  For i = 6 To n Step 3
    If Mid$(b$, i, 1) <> " " Then Exit Sub
    Next i
  If Not IsHex(Left$(b$, 4)) Then Exit Sub
  addr = Val("&h" & Left$(b$, 4))
  If addr > aTop Then Exit Sub
  
  For i = 7 To n Step 3
    If (i + 1) > n Then             ' partial hex number
      num = 0
      Exit Sub
    End If
    c$ = Mid$(b$, i, 2)
    If IsHex(c$) Then
      aval = Val("&h" & c$)
    Else
      If c$ = "--" Or c$ = "  " Then
        aval = -1
      Else
        num = 0                     ' abandon ship
        Exit Sub
      End If
    End If
    num = num + 1
    v(num) = aval
    Next i
  
End Sub

Function IsHex(a$) As Boolean
  Dim n As Integer, i As Integer, chk As Integer
  
  n = Len(a$)
  IsHex = False
  If n > 0 Then
    For i = 1 To n
      chk = Asc(LCase$(Mid$(a$, i, 1)))
      If chk < 48 Or chk > 102 Then Exit Function
      If chk > 57 And chk < 97 Then Exit Function
      Next i
    IsHex = True
  End If
    
End Function

Sub MergeHex(w As Integer, aTop As Integer, e As Integer)
' merge an Intel Hex file into the fl (w = 0) or ee (w = 1) array
' aTop - maximum legal address
' e - returned error code, 0 = OK
'
'   intel hex format:
'      1 char start code (':')
'      2 char byte count
'      4 char address
'      2 char record type
'         00 - data
'         01 - end of file (last line only)
'         02 - 05 are extended addressing, treated as errors here
'      n char data (usually 32 - byte count typically = 0x10)
'      2 char checksum

  Dim n As Integer, chk$, ck As Integer, a$, bCnt As Integer
  Dim typ As Integer, aval As Long, cc As Integer
  Dim ad As Integer, i As Integer, hold(20) As Integer
  Dim v As Integer, aa As Integer, j As Integer

  e = 1
  Do While Not EOF(1)
    Line Input #1, a$             ' read a line of hex file
    If Left$(a$, 1) <> ":" Then Exit Sub  ' bad format
    n = Len(a$)
    If (n And 1) = 0 Then Exit Sub    ' bad format
    If Not IsHex(Mid$(a$, 2)) Then Exit Sub   ' bad format
    
    chk$ = Right$(a$, 2)
    ck = Val("&H" & chk$)             ' the checksum value
    n = n - 3                         ' drop checksum and colon
    a$ = Mid$(a$, 2, n)

    cc = 0                        ' checksum calculation
    j = 0
    For i = 1 To n Step 2
      v = Val("&H" & Mid$(a$, i, 2))
      cc = cc + v
      j = j + 1
      hold(j) = v                 ' save value for later
      Next i
    ad = ck + cc
    ad = ad And 255
    If ad <> 0 Then
      e = 3                       ' checksum error
      Exit Sub
    End If
    
    bCnt = hold(1)                        ' byte count
    If bCnt > 20 Then Exit Sub            ' bad format
    aval = CLng(hold(2)) * 256 + hold(3)  ' address
    If aval > aTop Then aval = aTop + 1   ' keep things integral
    typ = hold(4)                         ' record type
    
    If n <> (bCnt + bCnt + 8) Then Exit Sub  ' bad format
    If typ < 0 Or typ > 2 Then        ' keep it simple
      e = 2                           ' bad record type
      Exit Sub
    End If
    
    If typ = 0 Then               ' only interested in data recs
      For i = 1 To bCnt
        aa = aval + i - 1         ' target address
        If aa <= aTop Then
          If w = 0 Then
            fl(aa) = hold(i + 4)
          Else
            ee(aa) = hold(i + 4)
          End If
        End If
        Next i
    End If
  Loop
  e = 0

End Sub

Sub ApplyPatches(f$, w As Integer, aTop As Integer, t As Integer)
' patch the image in fl (w = 0) or ee (w = 1) array
' f$ - file specification
' aTop - maximum legal address
' t = 0 for no update, = 1 for update after patching
  Dim i As Integer, j As Integer, v As Integer, ad As Integer
  
  Call ReadPatchFile(f$, w)
  If npatch = 0 Then Exit Sub
  
  For i = 1 To npatch
    With patch(i)
      For j = 1 To .nb            ' number of bytes to patch
        v = Asc(Mid$(.sval, j, 1))
        ad = .addr + j - 1        ' address for this one
        If ad <= aTop Then
          If w = 0 Then fl(ad) = v Else ee(ad) = v
        End If
        Next j
    End With
    Next i
  If t = 1 Then Call PatchPatchFile(f$)

End Sub

Sub ShowData(l As ListBox, w As Integer)
' put an ascii dump into listbox
' l - listbox to use
' w = 0 for flash, = 1 for eeprom
  Dim s$, i As Integer, a As Integer, aa As Integer, b$, v As Integer
  Dim ahi As Integer, t$, c$, r As Integer
  
  r = l.TopIndex
  If r > 60 And w = 1 Then r = 0
  l.Clear
  a = 0
  If w = 0 Then ahi = FLASHSIZE - 1 Else ahi = EEPROMSIZE - 1
  
  For a = 0 To ahi Step 8
    s$ = Right$("000" & Hex$(a), 4) & ")" & String$(24, " ")
    t$ = String$(8, ".")
    For i = 0 To 7
      aa = a + i
      If w = 0 Then v = fl(aa) Else v = ee(aa)
      c$ = "."
      If v < 0 Then             ' -1 is undefined
        b$ = "--"
      Else                      ' 0 - 255 is good
        If v > 31 And v < 128 Then c$ = Chr$(v)  ' printable?
        b$ = Right$("0" & Hex$(v), 2)
      End If
      Mid$(s$, i * 3 + 7, 2) = b$
      Mid$(t$, i + 1, 1) = c$
      Next i
    l.AddItem s$ & "  " & t$
    Next a
  l.TopIndex = r
 
End Sub

Sub ClearFl()
' set the flash image to undefined
  Dim i As Integer
  
  For i = 0 To FLASHSIZE - 1
    fl(i) = -1
    Next i
    
End Sub

Sub ClearEE()
' set the EEPROM image to undefined
  Dim i As Integer
  
  For i = 0 To EEPROMSIZE - 1
    ee(i) = -1
    Next i
    
End Sub

Sub AsciiDump(w As Integer, aTop As Integer)
' write an ascii dump to file #1
' w = 0 for flash, = 1 for eeprom
' aTop is the high address
' returns: 0 if ok, or error number
'
' format is AAAA) VV VV VV VV VV VV VV VV  ........
' undefined values will write as "--"
  Dim s$, i As Integer, a As Integer, b$, v As Integer, c$
  Dim aa As Integer
  
  For a = 0 To aTop Step 8
    s$ = Right$("000" & Hex$(a), 4) & ")" & String$(24, " ")
    c$ = String$(8, ".")
    For i = 0 To 7
      aa = a + i
      If w = 0 Then v = fl(aa) Else v = ee(aa)
      If v < 0 Then
        b$ = "--"
      Else
        b$ = Right$("0" & Hex$(v), 2)
      End If
      Mid$(s$, i * 3 + 7, 2) = b$
      If v > 31 And v < 128 Then Mid$(c$, i + 1, 1) = Chr$(v)
      Next i
    Print #1, s$ & "  " & c$
    Next a

End Sub

Function IntelHex(w As Integer, aTop As Integer) As Integer
' write Intel Hex to file #1
' w = 0 for flash, = 1 for eeprom
' returns: 0 if ok, or error number
'
' :02 0000 02 0000 FC
' :10 0000 00 0C94AB18189518950C947F070C94A107 C5
' ...
' :07 37C0 00 00000000000000 02
' :00 0000 01 FF
  Dim add1 As Integer, add2 As Integer, rtn As Integer

  On Error GoTo badrat
  Print #1, ":020000020000FC"
  add1 = 0                             ' initial range is everything
  add2 = aTop
  rtn = 1
  
  Do While add1 <= add2
    Call NextInt(w, add1, add2)        ' find next interval
    Call OutLine(w, add1, add2)        ' write the line(s)
    add1 = add2 + 1
    add2 = aTop
  Loop
  Print #1, ":00000001FF"
  IntelHex = 0
  Exit Function

badrat:
  IntelHex = Err.Number

End Function

Sub NextInt(w As Integer, a1 As Integer, a2 As Integer)
' find the next contiguous block of defined memory in image
' w - if 0, use fl(); otherwise use ee()
' a1,a2 - incoming, limit of addresses to check
'       - returned, limits of next defined block (array value <> 0)
' if no block is found, return a1 = a2 + 1
  Dim i As Integer, c As Integer, v As Integer, r1 As Integer
  
  c = 0                               ' looking for defined
  For i = a1 To a2
    If w = 0 Then v = fl(i) Else v = ee(i)
    If v >= 0 Then
      If c = 0 Then
        c = 1                         ' looking for undefined
        r1 = i                        ' save first defined
      End If
    Else
      If c = 1 Then
        a1 = r1                       ' first defined
        a2 = i - 1                    ' last defined
        Exit Sub
      End If
    End If
    Next i
  
  If c = 0 Then
    a1 = a2 + 1                       ' none in range - sorry
  Else
    a1 = r1                           ' first defined, a2 is ok as is
  End If

End Sub

Sub OutLine(w, a1 As Integer, a2 As Integer)
' write a line of intel hex
  Dim s$, siz As Integer, i As Integer, j As Integer
  Dim v As Integer, addr As Integer, chk As Integer
  
  If a1 > a2 Then Exit Sub                  ' no block found
  For i = a1 To a2 Step 16
    s$ = Right$("000" & Hex$(i), 4) & "00"  ' address & rec type 00
    chk = Val("&H" & Mid$(s$, 1, 2)) + Val("&H" & Mid$(s$, 3, 2))
    siz = a2 - i + 1
    If siz > 16 Then siz = 16
    s$ = ":" + Right$("0" & Hex$(siz), 2) & s$
    chk = chk + siz
    For j = 1 To siz
      addr = i + j - 1
      If w = 0 Then v = fl(addr) Else v = ee(addr)
      s$ = s$ & Right$("0" & Hex$(v), 2)
      chk = chk + v
      Next j
    chk = 256 - (chk And 255)       ' 0-->0, 1-->255, ..., 255-->1
    s$ = s$ & Right$("0" & Hex$(chk), 2)
    Print #1, s$
    Next i
    
End Sub

Function GetProfile$(sect$, key$, dflt$)
' read a private profile string. call GetPath() to initialize.
  Dim rtn$, r As Integer
  
  rtn$ = String$(300, 0)
  r = GetPrivateProfileString(sect$, key$, dflt$, rtn$, 296, ini_file$)
  If r > 0 Then rtn$ = Left$(rtn$, r) Else rtn$ = dflt$
  GetProfile$ = rtn$

End Function

Sub PutProfile(sect$, key$, v$)
' write a private profile string. call GetPath() to initialize.
  Dim rtn As Integer
  
  rtn = WritePrivateProfileString(sect$, key$, v$, ini_file$)

End Sub

Sub GetPath()
' find out where we are and set up ini file
  Dim basepath$
  
  basepath$ = App.Path
  If Right$(basepath$, 1) <> "/" Then basepath$ = basepath$ & "\"
  ini_file$ = basepath$ & "WIZnet_001153.ini"

End Sub

