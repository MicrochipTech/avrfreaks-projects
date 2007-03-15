'===============================================================================
'Library for the HP/Agilent/Avogo HCMS-29xx display for BASCOM-AVR

'This is the second of two .bas files that must be used together.
'Please see HCMSinit.bas for instructions.

'===============================================================================

Sub Hcmsreset
   Hcmsre = 0
   Waitus 250
   Hcmsre = 1
End Sub

Sub Hcmsconfigure(byval Hcmsbrightness As Byte)

   Local Hcmscontrolword As Byte
   Hcmscontrolword = &B01000000 + Hcmsbrightness

   Hcmsrs = 1                                               'Set Register Select to 1 for control register
   Waitus 250
   Hcmsce = 0                                               'Display chip enable
   Waitus 250
   Spiout Hcmscontrolword , 1                               'Send control word 0 to set brightness
   Waitus 250
   Hcmsce = 1                                               'Display chip disable
End Sub

Sub Hcmsclear                                               'Call Hcmsprint( "    ") will give the same result

   Local Hcmspixelloop As Word                              'Loop variable for the pixels
   Local Hcmsblank As Byte

   Hcmsblank = &H00
   Hcmsrs = 0                                               'Set Register Select to 0 for dot register
   Waitus 250
   Hcmsce = 0                                               'Display chip enable
   Waitus 250
   For Hcmspixelloop = 1 To 20                              'Send 20 blank columns
      Spiout Hcmsblank , 1                                  'Change to 40 for the 8-character displays
      Waitus 250
   Next Hcmspixelloop
   Hcmsce = 1                                               'Display chip disable
End Sub

Sub Hcmssleep                                               'Power-down mode
   Hcmscommandout &B00001001
End Sub

Sub Hcmscommandout(byval Hcmsvalue As Byte)
   Hcmsrs = 1                                               'Set Register Select to 1 for control register
   Waitus 250
   Hcmsce = 0                                               'Display chip enable
   Waitus 250
   Spiout Hcmsvalue , 1
   Waitus 250
   Hcmsce = 1                                               'Display chip disable
End Sub

Sub Hcmsprint(byval Hcmsstring As String * 4)               'Change to 8 for the 8-character displays

   Local Hcmslength As Byte                                 'Holds the length of HCMSstring
   Local Hcmscharacter As String * 1                        'Holds each consecutive character in turn during display output
   Local Hcmsstringloop As Byte                             'Loop variable for the string
   Local Hcmscolumnloop As Byte                             'Loop variable for the columns
   Local Hcmscolumn As Byte                                 'Display column output

   Hcmslength = Len(hcmsstring)                             'Establish the number of characters

   Hcmsrs = 0                                               'Set Register Select to 0 for dot register
   Waitus 250
   Hcmsce = 0                                               'Display chip enable
   Waitus 250

   For Hcmsstringloop = 1 To Hcmslength                     'Loop through the string

      Hcmscharacter = Mid(hcmsstring , Hcmsstringloop , 1)  'Get the current character

      Select Case Hcmscharacter                             'Position the pointer to the correct character in the Eeprom and read the first byte
         Case "0" : Restore Zero
         Case "1" : Restore One
         Case "2" : Restore Two
         Case "3" : Restore Three
         Case "4" : Restore Four
         Case "5" : Restore Five
         Case "6" : Restore Six
         Case "7" : Restore Seven
         Case "8" : Restore Eight
         Case "9" : Restore Nine
         Case "A" : Restore A
         Case "B" : Restore B
         Case "C" : Restore C
         Case "D" : Restore D
         Case "E" : Restore E
         Case "F" : Restore F
         Case "G" : Restore G
         Case "H" : Restore H
         Case "I" : Restore I
         Case "J" : Restore J
         Case "K" : Restore K
         Case "L" : Restore L
         Case "M" : Restore M
         Case "N" : Restore N
         Case "O" : Restore O
         Case "P" : Restore P
         Case "Q" : Restore Q
         Case "R" : Restore R
         Case "S" : Restore S
         Case "T" : Restore T
         Case "U" : Restore U
         Case "V" : Restore V
         Case "W" : Restore W
         Case "X" : Restore X
         Case "Y" : Restore Y
         Case "Z" : Restore Z
         Case "Å" : Restore Aa
         Case "Ä" : Restore Ae
         Case "Ö" : Restore Oe
         Case "a" : Restore A1
         Case "b" : Restore B1
         Case "c" : Restore C1
         Case "d" : Restore D1
         Case "e" : Restore E1
         Case "f" : Restore F1
         Case "g" : Restore G1
         Case "h" : Restore H1
         Case "i" : Restore I1
         Case "j" : Restore J1
         Case "k" : Restore K1
         Case "l" : Restore L1
         Case "m" : Restore M1
         Case "n" : Restore N1
         Case "o" : Restore O1
         Case "p" : Restore P1
         Case "q" : Restore Q1
         Case "r" : Restore R1
         Case "s" : Restore S1
         Case "t" : Restore T1
         Case "u" : Restore U1
         Case "v" : Restore V1
         Case "w" : Restore W1
         Case "x" : Restore X1
         Case "y" : Restore Y1
         Case "z" : Restore Z1
         Case "å" : Restore Aa1
         Case "ä" : Restore Ae1
         Case "ö" : Restore Oe1
         Case " " : Restore Spacesign
         Case "-" : Restore Dash
         Case "." : Restore Dot
         Case ":" : Restore Colon
         Case ">" : Restore Arrowright
         Case "°" : Restore Degree
         Case "@" : Restore Alpha
         Case "&" : Restore Ampersand
         Case "'" : Restore Apostrophe
         Case "<" : Restore Arrowleft
         Case "*" : Restore Asterisc
         Case "," : Restore Comma
         Case "$" : Restore Dollar
         Case "=" : Restore Equals
         Case "!" : Restore Exclamation
         Case "½" : Restore Grid
         Case "#" : Restore Hash
         Case "(" : Restore Leftparagraph
         Case "[" : Restore Leftsquareparagraph
         Case "%" : Restore Percent
         Case "+" : Restore Plus
         Case "^" : Restore Powerof
         Case "?" : Restore Question
         Case Hcmsquotation : Restore Quotation
         Case "]" : Restore Rightparagraph
         Case ")" : Restore Rightsquareparagraph
         Case ";" : Restore Semicolon
         Case "/" : Restore Slash
         Case "\" : Restore Backslash
         Case "_" : Restore Underscore

      End Select

         For Hcmscolumnloop = 1 To 5                        'Read and print each column
            Read Hcmscolumn
            Spiout Hcmscolumn , 1                           'Send the column
            Waitus 250
         Next Hcmscolumnloop

   Next Hcmsstringloop                                      'Continue with the next character

   Hcmsce = 1                                               'Display chip disable latches the dot register to the LEDs

End Sub

'===============================================================================

'ASCII data

'Digits
Zero:
Data &H3E , &H51 , &H49 , &H45 , &H3E
One:
Data &H00 , &H42 , &H7F , &H40 , &H00
Two:
Data &H42 , &H61 , &H51 , &H49 , &H46
Three:
Data &H22 , &H41 , &H49 , &H49 , &H36
Four:
Data &H18 , &H14 , &H12 , &H7F , &H10
Five:
Data &H27 , &H45 , &H45 , &H45 , &H39
Six:
Data &H3E , &H49 , &H49 , &H49 , &H32
Seven:
Data &H61 , &H11 , &H09 , &H05 , &H03
Eight:
Data &H36 , &H49 , &H49 , &H49 , &H36
Nine:
Data &H26 , &H49 , &H49 , &H49 , &H3E

'Upper-case
A:
Data &H7E , &H09 , &H09 , &H09 , &H7E
B:
Data &H7F , &H49 , &H49 , &H49 , &H36
C:
Data &H3E , &H41 , &H41 , &H41 , &H22
D:
Data &H7F , &H41 , &H41 , &H41 , &H3E
E:
Data &H7F , &H49 , &H49 , &H49 , &H41
F:
Data &H7F , &H09 , &H09 , &H09 , &H01
G:
Data &H3E , &H41 , &H49 , &H49 , &H3A
H:
Data &H7F , &H08 , &H08 , &H08 , &H7F
I:
Data &H00 , &H41 , &H7F , &H41 , &H00
J:
Data &H20 , &H41 , &H41 , &H41 , &H3F
K:
Data &H7F , &H08 , &H14 , &H22 , &H41
L:
Data &H7F , &H40 , &H40 , &H40 , &H40
M:
Data &H7F , &H02 , &H04 , &H02 , &H7F
N:
Data &H7F , &H04 , &H08 , &H10 , &H7F
O:
Data &H3E , &H41 , &H41 , &H41 , &H3E
P:
Data &H7F , &H09 , &H09 , &H09 , &H06
Q:
Data &H3E , &H41 , &H51 , &H61 , &H7E
R:
Data &H7F , &H09 , &H19 , &H29 , &H46
S:
Data &H06 , &H49 , &H49 , &H49 , &H30
T:
Data &H01 , &H01 , &H7F , &H01 , &H01
U:
Data &H3F , &H40 , &H40 , &H40 , &H3F
V:
Data &H1F , &H20 , &H40 , &H20 , &H1F
W:
Data &H7F , &H20 , &H10 , &H20 , &H7F
X:
Data &H63 , &H14 , &H08 , &H14 , &H63
Y:
Data &H03 , &H04 , &H78 , &H04 , &H03
Z:
Data &H61 , &H51 , &H49 , &H45 , &H43
Aa:
Data &H78 , &H14 , &H15 , &H14 , &H78
Ae:
Data &H78 , &H15 , &H14 , &H15 , &H78
Oe:
Data &H38 , &H45 , &H44 , &H45 , &H38

'Lower-case
A1:
Data &H20 , &H54 , &H54 , &H54 , &H78
B1:
Data &H7F , &H48 , &H44 , &H44 , &H38
C1:
Data &H38 , &H44 , &H44 , &H44 , &H28
D1:
Data &H38 , &H44 , &H44 , &H48 , &H7F
E1:
Data &H38 , &H54 , &H54 , &H54 , &H18
F1:
Data &H00 , &H04 , &H7E , &H05 , &H00
G1:
Data &H0C , &H52 , &H52 , &H4A , &H3C
H1:
Data &H7F , &H08 , &H04 , &H04 , &H78
I1:
Data &H00 , &H44 , &H7D , &H40 , &H00
J1:
Data &H00 , &H40 , &H44 , &H3D , &H00
K1:
Data &H7F , &H20 , &H10 , &H28 , &H44
L1:
Data &H00 , &H41 , &H7F , &H40 , &H00
M1:
Data &H7C , &H04 , &H18 , &H04 , &H7C
N1:
Data &H7C , &H08 , &H04 , &H04 , &H78
O1:
Data &H38 , &H44 , &H44 , &H44 , &H38
P1:
Data &H7C , &H14 , &H24 , &H24 , &H18
Q1:
Data &H18 , &H24 , &H24 , &H14 , &H7C
R1:
Data &H7C , &H04 , &H04 , &H08 , &H00
S1:
Data &H08 , &H54 , &H54 , &H54 , &H20
T1:
Data &H04 , &H3F , &H44 , &H40 , &H00
U1:
Data &H3C , &H40 , &H40 , &H40 , &H7C
V1:
Data &H1C , &H20 , &H40 , &H20 , &H1C
W1:
Data &H3C , &H40 , &H30 , &H40 , &H3C
X1:
Data &H44 , &H28 , &H10 , &H28 , &H44
Y1:
Data &H0C , &H50 , &H50 , &H48 , &H3C
Z1:
Data &H44 , &H64 , &H54 , &H4C , &H44
Aa1:
Data &H20 , &H54 , &H55 , &H54 , &H78
Ae1:
Data &H20 , &H55 , &H54 , &H55 , &H78
Oe1:
Data &H38 , &H45 , &H44 , &H45 , &H38

'Special characters
Spacesign:
Data &H00 , &H00 , &H00 , &H00 , &H00
Dot:
Data &H00 , &H60 , &H60 , &H00 , &H00
Dash:
Data &H08 , &H08 , &H08 , &H08 , &H08
Colon:
Data &H00 , &H36 , &H36 , &H00 , &H00
Degree:
Data &H00 , &H07 , &H05 , &H07 , &H00
Leftsquareparagraph:
Data &H00 , &H7F , &H41 , &H41 , &H00
Rightsquareparagraph:
Data &H00 , &H41 , &H41 , &H7F , &H00
Grid:
Data &H55 , &H2A , &H55 , &H2A , &H55
Powerof:
Data &H04 , &H02 , &H01 , &H02 , &H04
Underscore:
Data &H40 , &H40 , &H40 , &H40 , &H40
Semicolon:
Data &H00 , &H56 , &H36 , &H00 , &H00
Arrowleft:
Data &H08 , &H1C , &H36 , &H63 , &H41
Arrowright:
Data &H41 , &H63 , &H36 , &H1C , &H08
Equals:
Data &H14 , &H14 , &H14 , &H14 , &H14
Question:
Data &H02 , &H01 , &H51 , &H09 , &H06
Alpha:
Data &H32 , &H49 , &H59 , &H51 , &H3E
Exclamation:
Data &H00 , &H00 , &H5F , &H00 , &H00
Quotation:
Data &H00 , &H07 , &H00 , &H07 , &H00
Hash:
Data &H14 , &H7F , &H14 , &H7F , &H14
Dollar:
Data &H24 , &H2A , &H7F , &H2A , &H12
Percent:
Data &H23 , &H13 , &H08 , &H64 , &H62
Ampersand:
Data &H36 , &H49 , &H55 , &H22 , &H50
Apostrophe:
Data &H00 , &H05 , &H03 , &H00 , &H00
Leftparagraph:
Data &H00 , &H1C , &H22 , &H41 , &H00
Rightparagraph:
Data &H00 , &H41 , &H22 , &H1C , &H00
Asterisc:
Data &H14 , &H08 , &H3E , &H08 , &H14
Plus:
Data &H08 , &H08 , &H3E , &H08 , &H08
Comma:
Data &H00 , &H00 , &H50 , &H30 , &H00
Slash:
Data &H20 , &H10 , &H08 , &H04 , &H02
Backslash:
Data &H02 , &H04 , &H08 , &H10 , &H20