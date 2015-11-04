
DECLARE FUNCTION tanc# (xx AS DOUBLE)
DECLARE FUNCTION cosc# (xx AS DOUBLE)
DECLARE FUNCTION sinc# (xx AS DOUBLE)
'+-------------------------------------------------+
'| ALGORITHMS TO PERFORM TRIGONOMETRICS FUNCTIONS  |
'| BY JOAO DARTAGNAN ANTUNES OLIVEIRA              |
'| DATE: 15/06/2001                                |
'+-------------------------------------------------+
DEFDBL A-Z
COLOR , 1
CLS
PRINT sinc(.5#)
PRINT ATN(.5#), ATN(1)


'FOR x = -1000 TO 1000 STEP 10
'        PRINT x, tanc(x), TAN(x)
'NEXT

FUNCTION cosc (xx AS DOUBLE)
pi2 = ATN(1#) * 2
        cosc = sinc(xx + pi2)
END FUNCTION

FUNCTION sinc (xx AS DOUBLE) STATIC
'+---------------------------------------+
'| sine algorithm using maclarium series |
'|                                       |
'|               3     5     7     9     |
'|              x     x     x     x      |
'| sin x = 1 - --- + --- - --- + --- ....|
'|              3!    5!    7!    9!     |
'+---------------------------------------+

'
'error if xx>1d9
'
pi2 = ATN(1) * 2                        'pi2 constant
        x = xx
        schange = 1                     '1bit field
        IF x < 0 THEN
           schange = -1
        END IF                          'double
        x = ABS(x)
        x = x / pi2
        ss = INT(x) MOD 4               '2bit field
        x = (x - INT(x)) * pi2
        SELECT CASE ss
            CASE 0
               sout = 1
            CASE 1
               x = pi2 - x
               sout = 1
            CASE 2
               sout = -1
            CASE 3
               x = pi2 - x
               sout = -1
        END SELECT
        f = 1                           'double
        c = 1                           'integer
        e = -x * x                      'double
        s = 0                           'double
        FOR k = 1 TO 24
                s = s + x / f
                x = x * e
                c = c + 2
              '  f = f * c * (c - 1)
              f = c
        NEXT
        IF schange = 1 THEN
                sinc = sout * s
        ELSE
                sinc = sout * s * -1
        END IF
       
END FUNCTION

FUNCTION tanc (xx AS DOUBLE)
        tanc = sinc(xx) / cosc(xx)
END FUNCTION

