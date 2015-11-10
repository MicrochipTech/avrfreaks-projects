'+-------------------------------------+
'| DFT AND IDFT FOR STUDY PURPOSE      |
'| BY JOAO DARTAGNAN ANTUNES OLIVEIRA  |
'| DATE: 30/04/2001                    |
'+-------------------------------------+

CONST POINTS = 64               'DFT NUMBER POINTS
DIM F(0 TO POINTS) AS INTEGER   'TO STORE MY INPUT SAMPLE VALUES
DIM F.A(0 TO POINTS) AS INTEGER 'TO STORE FOURIER A PARMS
DIM F.B(0 TO POINTS) AS INTEGER 'TO STORE FOURIER B PARMS

PI = ATN(1) * 4

'+--------------------------------------
'| DEFINE MY INPUT FUNCTION
'+--------------------------------------

FOR i = 0 TO POINTS - 1
    W = i / POINTS * 2 * PI
    F(i) = 127 + 127 * (SIN(W) / 2)
    'IF I > (POINTS / 2) THEN F(I) = 127
NEXT

'+--------------------------------------
'| PLOT MY INPUT FUNTION
'+--------------------------------------
SCREEN 12

LOCATE 1, 1
PRINT "Input Function"
PSET (0, 120)
FOR i = 0 TO POINTS - 1
   LINE -(i / POINTS * 320, F(i)), 12
NEXT

'+--------------------------------------
'| START MY DFT COMPUTATION
'+--------------------------------------

FOR i = 1 TO POINTS / 2 - 1
        A = 0                   'CLEAR FOURIER A & B PARAMETERS
        B = 0
        FOR j = 0 TO POINTS - 1
                W = j / POINTS * 2 * PI * i
                A = A + F(j) * COS(W)
                B = B + F(j) * SIN(W)
        NEXT
        F.A(i) = 2 * A / POINTS
        F.B(i) = 2 * B / POINTS
NEXT

'FOR i = 1 TO 7
'        f.a(i) = 0
'        F.B(i) = 0
'NEXT

'F.A(2) = 0
'F.B(2) = 0
'F.A(8) = 0
'F.B(8) = 0
'F.A(50) = 0
'F.B(50) = 0

'+--------------------------------------
'| PLOT FOURIER PARAMETERS
'+--------------------------------------

FOR i = 1 TO POINTS / 2 - 1
        XI = i / POINTS * 640 + 320
        XF = XI + .5 / POINTS * 320
        LINE (XI, 60)-(XF, 60 - F.A(i) / 128 * 60), 10, BF
        LINE (XI, 180)-(XF, 180 - F.B(i) / 128 * 60), 14, BF
        P = SQR(F.A(i) ^ 2 + F.B(i) ^ 2)
        LINE (XI, 479)-(XF, 479 - P / 128 * 240), 11, BF
NEXT

'+--------------------------------------
'| PLOT INVERSE FOURIER FUNCTION
'+--------------------------------------

LOCATE 16, 1
PRINT "Inverse Fourier Function"
PSET (0, 360)
FOR i = 0 TO POINTS - 1
        C = 0
        S = 0
        FOR j = 1 TO POINTS / 2 - 1
                W = i / POINTS * 2 * PI * j
                C = C + F.A(j) * COS(W)
                S = S + F.B(j) * SIN(W)
        NEXT
        LINE -(i / POINTS * 320, 360 + (C + S)), 9
NEXT

'+--------------------------------------
'| DETAILS
'+--------------------------------------

LOCATE 1, 42
PRINT "A Parms"
LOCATE 9, 42
PRINT "B Parms"
LOCATE 16, 42
PRINT "Power Spectrum"
LINE (320, 0)-(320, 479), 7
LINE (320, 120)-(639, 120), 7
LINE (0, 240)-(639, 240), 7
W$ = INPUT$(1)
END

