'+------------------------------------------+
'| String to Float point conversion routine |
'| by Joao Dartagnan A. Oliveira  12/06/2001|
'+------------------------------------------+

CLS
DEFDBL A-Z
DIM bb%(0 TO 7)

INPUT "Number "; Number$

Number$ = Number$ + CHR$(0)

id.s% = 0      'bit flag
m.s% = 1       'bit flag
e.s% = 1       'bit flag
d.p% = 1       'bit flag
e.p% = 1       'bit flag
m.ac = 0       'double mantisa acumulator
e.ac = 0       'byte exponent count
pd = 10        'double
i% = 1

DO WHILE MID$(Number$, i%, 1) <> CHR$(0)

    ch$ = MID$(Number$, i%, 1)
    SELECT CASE ch$
        CASE "+", "-"                   'Mantisa and Exponent Signal
          IF id.s% = 0 THEN             'ids=1 mantisa signal
             IF ch$ = "-" THEN
                   m.s% = -1
             END IF
             id.s% = -1
          ELSE                          'ids=-1 exponent signal
             IF ch$ = "-" THEN
                   e.s% = -1
             END IF
          END IF
        CASE "."
          d.p% = -1                     'd.p=-1 sinalizing point
        CASE "E", "e"
          e.p% = -1                     'e.p=-1 sinalizing exponent operator
          id.s% = -1                    'say that only exponent signal after this
        CASE "0" TO "9"
          IF e.p% = 1 THEN              'e.p=1 extract mantisa digits
                  IF d.p% = 1 THEN
                     m.ac = m.ac * 10 + VAL(ch$)
                  ELSE
                     m.ac = m.ac + VAL(ch$) / pd
                     pd = pd * 10
                  END IF
          ELSE
                  e.ac = e.ac * 10 + VAL(ch$) 'e.p=-1 extract exponent digits
          END IF
    END SELECT
    i% = i% + 1
LOOP
FOR i% = 1 TO e.ac
    IF e.s% = 1 THEN           'multiply mantisa by 10 e.ac times
       m.ac = m.ac * 10
    ELSE
       m.ac = m.ac / 10        'divide mantisa by 10 e.ac times
    END IF
NEXT
IF m.s% = -1 THEN m.ac = -m.ac

'
'check                 
'
PRINT m.ac
GOSUB verhexa
m.ac = VAL(Number$)
GOSUB verhexa
END
verhexa:
k% = 0
FOR i = VARPTR(m.ac) TO VARPTR(m.ac) + 7
        bb%(k%) = PEEK(i)
        k% = k% + 1
NEXT
tp% = 0
FOR z% = 1 TO 3
FOR i = 0 TO k% - 1
        tp% = bb%(i) * 2 + tp% \ 256
        bb%(i) = tp% AND &HFF
NEXT
NEXT
FOR i = 0 TO k% - 1
        PRINT RIGHT$("00" + HEX$(bb%(i)), 2); " ";
NEXT
PRINT
RETURN

