CLS
'+---------------------------------------------------+
'| Algoritimo para conversao de ponto flutuante      |
'| em string sem formatacao ainda zero nao checado   |
'| sem arredondamento da saida de 16 para 15 digitos |
'| versao 1.0                                        |
'| by Joao Dartagnan Antunes Oliveira Date:21/06/2001|
'+---------------------------------------------------+
DEFDBL A-Z
INPUT x
IF x < 0 THEN s% = 1 ELSE s% = 0
x = ABS(x)
IF x <> 0 THEN
        DO WHILE x < 1D+16
             x = x * 10#
             e = e - 1#
        LOOP
        DO WHILE x >= 1D+16
             x = x / 10
             e = e + 1#
        LOOP
END IF
p10 = 1D+16
f$ = ""
DO
   c = -1#
   DO
     c = c + 1#
     x = x - p10
   LOOP UNTIL x < 0
   f$ = f$ + RTRIM$(LTRIM$(STR$(c)))
   x = x + p10
   p10 = p10 / 10#
LOOP UNTIL p10 < 1#
f$ = RIGHT$(f$, LEN(f$) - 1)
e = e + 15
IF s% = 1 THEN f$ = "-" + f$ ELSE f$ = "+" + f$
PRINT f$; "D"; RTRIM$(LTRIM$(STR$(e)))

