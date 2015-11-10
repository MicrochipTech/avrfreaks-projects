'+----------------------------------------+
'| GET CLOSED MATH DIVISORS FOR TIMER0    |
'| RETURN PRESCALER VALUE,TIMER DIVISION  |
'+----------------------------------------+
CLS
DIM FCLK AS LONG, FGEN AS LONG
DIM R AS LONG, K AS LONG

FCLK = 6144000
FGEN = 100

DIM PRESCALER(1 TO 5) AS INTEGER
PRESCALER(1) = 1
PRESCALER(2) = 8
PRESCALER(3) = 64
PRESCALER(4) = 256
PRESCALER(5) = 1024

FOR I = 1 TO 5
        R = FCLK / PRESCALER(I) / FGEN
        'R = R / FGEN
        IF R > 0 AND R < 256 THEN
                K = FCLK / PRESCALER(I)
                K = K / R
                PRINT K, PRESCALER(I), R
                EXIT FOR
        END IF
NEXT

