'+---------------------------------------+
'|                                       |
'| SPRINTF WITH MASK FOR INTEGER NUMBER  |
'|                                       |
'| By Joao Dartagnan Antunes Oliveira    |
'|                                       |
'| SAMPLES                               |
'| MASK           NUMBER    RESULT       |
'|                                       |
'| TESTE +######  34        TESTE     34 |
'| TESTE +######  122       TESTE -  122 |
'| TESTE -######  34        TESTE +   34 |
'| TESTE -######  122       TESTE -  122 |
'| FRAC  +##0.00  15        FRAC    0.15 |
'| FRAC  +##0.00  -1        FRAC  - 0.01 |
'| FRAC  -##0.00  123       FRAC  + 1.23 |
'| FRAC  -##0.00  -1156     FRAC  -11.56 |
'|                                       |
'+---------------------------------------+
CLS

'DEFINE MY MASK

MASK$ = "+####0.00"

'DEFINE MY NUMBER

'INPUT "Number "; n$

n$ = "67"

IF VAL(n$) < 0 THEN negative = 1 ELSE negative = 0

n$ = RTRIM$(LTRIM$(STR$(ABS(VAL(n$)))))

sizeN = LEN(n$)
sizeD = 0

'EXTRACT SIZE OF 0+# AND SEE IF PLUS SIGN EXIST


FOR i = 1 TO LEN(MASK$)
        SELECT CASE MID$(MASK$, i, 1)
            CASE "#", "0"
                sizeD = sizeD + 1
            CASE "-"
                IF negative = 0 THEN
                   MID$(MASK$, i, 1) = "+"
                END IF
            CASE "+"
                IF negative = 1 THEN
                   MID$(MASK$, i, 1) = "-" 'insert negative if so
                ELSE
                   MID$(MASK$, i, 1) = " "
                END IF
        END SELECT
NEXT

'COMPUTE TOTAL SIZE

sizeT = sizeN - sizeD

'PROCESS NUMBER INSERTION INTO MASK

FOR i = 1 TO LEN(MASK$)
    SELECT CASE MID$(MASK$, i, 1)
        CASE "#", "0"
             IF sizeT >= 0 THEN
                 DO
                   t% = 1
                 LOOP UNTIL t%
                 MID$(MASK$, i, 1) = MID$(n$, sizeT + 1, 1)
             ELSE
                 IF MID$(MASK$, i, 1) = "#" THEN
                     MID$(MASK$, i, 1) = " "
                 ELSE
                     MID$(MASK$, i, 1) = "0"
                 END IF
             END IF
             sizeT = sizeT + 1
        CASE ELSE
    END SELECT
NEXT

'SHOW RESULT

PRINT MASK$

