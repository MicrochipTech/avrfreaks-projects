280: Error: Invalid line
+00000080:   9563        INC     R22              Increment
281: Error: Invalid line
+00000081:   156B        CP      R22,R11          Compare
282: Error: Invalid line
+00000082:   F799        BRNE    PC-0x0C          Branch if not equal
@00000083: increment_row
284: Error: Invalid line
+00000083:   9553        INC     R21              Increment
285: Error: Invalid line
+00000084:   155D        CP      R21,R13          Compare
286: Error: Invalid line
+00000085:   F769        BRNE    PC-0x12          Branch if not equal
288: Error: Invalid line
+00000086:   2D5C        MOV     R21,R12          Copy register
289: Error: Invalid line
+00000087:   2D6A        MOV     R22,R10          Copy register
290: Error: Invalid line
+00000088:   D053        RCALL   PC+0x0054        Relative call subroutine
292: Error: Invalid line
+00000089:   7F7D        ANDI    R23,0xFD         Logical AND with immediate
293: Error: Invalid line
+0000008A:   9508        RET                      Subroutine return
@0000008B: SEND_SCROLL
297: Error: Invalid line
+0000008B:   D02A        RCALL   PC+0x002B        Relative call subroutine
298: Error: Invalid line
+0000008C:   D014        RCALL   PC+0x0015        Relative call subroutine
299: Error: Invalid line
+0000008D:   6C20        ORI     R18,0xC0         Logical OR with immediate
300: Error: Invalid line
+0000008E:   BB25        OUT     0x15,R18         Out to I/O location
301: Error: Invalid line
+0000008F:   D032        RCALL   PC+0x0033        Relative call subroutine
303: Error: Invalid line
+00000090:   9508        RET                      Subroutine return
@00000091: SCROLL_DISPLAY
307: Error: Invalid line
+00000091:   DFF9        RCALL   PC-0x0006        Relative call subroutine
@00000092: CLEAR_LINE
310: Error: Invalid line
+00000092:   936F        PUSH    R22              Push register on stack
311: Error: Invalid line
+00000093:   2D6A        MOV     R22,R10          Copy register
312: Error: Invalid line
+00000094:   9180008E    LDS     R24,0x008E       Load direct from data space
@00000096: NEXT_COLUMN_CLEAR
314: Error: Invalid line
+00000096:   D045        RCALL   PC+0x0046        Relative call subroutine
315: Error: Invalid line
+00000097:   2D08        MOV     R16,R8           Copy register
@00000098: NEXT_BYTE
317: Error: Invalid line
+00000098:   BB85        OUT     0x15,R24         Out to I/O location
318: Error: Invalid line
+00000099:   D028        RCALL   PC+0x0029        Relative call subroutine
319: Error: Invalid line
+0000009A:   950A        DEC     R16              Decrement
320: Error: Invalid line
+0000009B:   F7E1        BRNE    PC-0x03          Branch if not equal
321: Error: Invalid line
+0000009C:   0D68        ADD     R22,R8           Add without carry
322: Error: Invalid line
+0000009D:   156B        CP      R22,R11          Compare
323: Error: Invalid line
+0000009E:   F3B8        BRCS    PC-0x08          Branch if carry set
324: Error: Invalid line
+0000009F:   916F        POP     R22              Pop register from stack
325: Error: Invalid line
+000000A0:   9508        RET                      Subroutine return
@000000A1: SEND_INSTRUCTION
330: Error: Invalid line
+000000A1:   98DC        CBI     0x1B,4           Clear bit in I/O register
331: Error: Invalid line
+000000A2:   98DD        CBI     0x1B,5           Clear bit in I/O register
332: Error: Invalid line
+000000A3:   9508        RET                      Subroutine return
@000000A4: SEND_DATA
335: Error: Invalid line
+000000A4:   9ADC        SBI     0x1B,4           Set bit in I/O register
336: Error: Invalid line
+000000A5:   98DD        CBI     0x1B,5           Clear bit in I/O register
337: Error: Invalid line
+000000A6:   9508        RET                      Subroutine return
@000000A7: FETCH_DATA
340: Error: Invalid line
+000000A7:   9ADC        SBI     0x1B,4           Set bit in I/O register
341: Error: Invalid line
+000000A8:   9ADD        SBI     0x1B,5           Set bit in I/O register
342: Error: Invalid line
+000000A9:   9508        RET                      Subroutine return
@000000AA: DISPLAY_ON
345: Error: Invalid line
+000000AA:   D00B        RCALL   PC+0x000C        Relative call subroutine
346: Error: Invalid line
+000000AB:   DFF5        RCALL   PC-0x000A        Relative call subroutine
347: Error: Invalid line
+000000AC:   E30F        LDI     R16,0x3F         Load immediate
348: Error: Invalid line
+000000AD:   BB05        OUT     0x15,R16         Out to I/O location
349: Error: Invalid line
+000000AE:   D013        RCALL   PC+0x0014        Relative call subroutine
350: Error: Invalid line
+000000AF:   9508        RET                      Subroutine return
@000000B0: DISPLAY_OFF
354: Error: Invalid line
+000000B0:   D005        RCALL   PC+0x0006        Relative call subroutine
355: Error: Invalid line
+000000B1:   DFEF        RCALL   PC-0x0010        Relative call subroutine
356: Error: Invalid line
+000000B2:   E30E        LDI     R16,0x3E         Load immediate
357: Error: Invalid line
+000000B3:   BB05        OUT     0x15,R16         Out to I/O location
358: Error: Invalid line
+000000B4:   D00D        RCALL   PC+0x000E        Relative call subroutine
359: Error: Invalid line
+000000B5:   9508        RET                      Subroutine return
@000000B6: TWO_SIDE
363: Error: Invalid line
+000000B6:   98DE        CBI     0x1B,6           Clear bit in I/O register
364: Error: Invalid line
+000000B7:   98DF        CBI     0x1B,7           Clear bit in I/O register
365: Error: Invalid line
+000000B8:   9508        RET                      Subroutine return
@000000B9: NO_SIDE
369: Error: Invalid line
+000000B9:   9ADE        SBI     0x1B,6           Set bit in I/O register
370: Error: Invalid line
+000000BA:   9ADF        SBI     0x1B,7           Set bit in I/O register
371: Error: Invalid line
+000000BB:   9508        RET                      Subroutine return
@000000BC: LEFT_SIDE
375: Error: Invalid line
+000000BC:   9ADE        SBI     0x1B,6           Set bit in I/O register
376: Error: Invalid line
+000000BD:   98DF        CBI     0x1B,7           Clear bit in I/O register
377: Error: Invalid line
+000000BE:   9508        RET                      Subroutine return
@000000BF: RIGHT_SIDE
381: Error: Invalid line
+000000BF:   9ADF        SBI     0x1B,7           Set bit in I/O register
382: Error: Invalid line
+000000C0:   98DE        CBI     0x1B,6           Clear bit in I/O register
383: Error: Invalid line
+000000C1:   9508        RET                      Subroutine return
@000000C2: CLOCK
387: Error: Invalid line
+000000C2:   9ADB        SBI     0x1B,3           Set bit in I/O register
388: Error: Invalid line
+000000C3:   0000        NOP                      No operation
389: Error: Invalid line
+000000C4:   0000        NOP                      No operation
390: Error: Invalid line
+000000C5:   0000        NOP                      No operation
391: Error: Invalid line
+000000C6:   0000        NOP                      No operation
392: Error: Invalid line
+000000C7:   0000        NOP                      No operation
393: Error: Invalid line
+000000C8:   0000        NOP                      No operation
394: Error: Invalid line
+000000C9:   0000        NOP                      No operation
395: Error: Invalid line
+000000CA:   0000        NOP                      No operation
396: Error: Invalid line
+000000CB:   0000        NOP                      No operation
397: Error: Invalid line
+000000CC:   0000        NOP                      No operation
398: Error: Invalid line
+000000CD:   0000        NOP                      No operation
399: Error: Invalid line
+000000CE:   0000        NOP                      No operation
400: Error: Invalid line
+000000CF:   0000        NOP                      No operation
401: Error: Invalid line
+000000D0:   98DB        CBI     0x1B,3           Clear bit in I/O register
402: Error: Invalid line
+000000D1:   0000        NOP                      No operation
403: Error: Invalid line
+000000D2:   0000        NOP                      No operation
404: Error: Invalid line
+000000D3:   0000        NOP                      No operation
405: Error: Invalid line
+000000D4:   0000        NOP                      No operation
406: Error: Invalid line
+000000D5:   0000        NOP                      No operation
407: Error: Invalid line
+000000D6:   0000        NOP                      No operation
408: Error: Invalid line
+000000D7:   0000        NOP                      No operation
409: Error: Invalid line
+000000D8:   0000        NOP                      No operation
410: Error: Invalid line
+000000D9:   0000        NOP                      No operation
411: Error: Invalid line
+000000DA:   0000        NOP                      No operation
412: Error: Invalid line
+000000DB:   9508        RET                      Subroutine return
@000000DC: TAB
417: Error: Invalid line
+000000DC:   930F        PUSH    R16              Push register on stack
418: Error: Invalid line
+000000DD:   DFC3        RCALL   PC-0x003C        Relative call subroutine
419: Error: Invalid line
+000000DE:   D004        RCALL   PC+0x0005        Relative call subroutine
420: Error: Invalid line
+000000DF:   D00F        RCALL   PC+0x0010        Relative call subroutine
421: Error: Invalid line
+000000E0:   DFC3        RCALL   PC-0x003C        Relative call subroutine
422: Error: Invalid line
+000000E1:   910F        POP     R16              Pop register from stack
423: Error: Invalid line
+000000E2:   9508        RET                      Subroutine return
@000000E3: SET_row
426: Error: Invalid line
+000000E3:   DFD2        RCALL   PC-0x002D        Relative call subroutine
427: Error: Invalid line
+000000E4:   2F02        MOV     R16,R18          Copy register
428: Error: Invalid line
+000000E5:   730F        ANDI    R16,0x3F         Logical AND with immediate
429: Error: Invalid line
+000000E6:   9505        ASR     R16              Arithmetic shift right
430: Error: Invalid line
+000000E7:   9505        ASR     R16              Arithmetic shift right
431: Error: Invalid line
+000000E8:   9505        ASR     R16              Arithmetic shift right
432: Error: Invalid line
+000000E9:   0F05        ADD     R16,R21          Add without carry
433: Error: Invalid line
+000000EA:   7007        ANDI    R16,0x07         Logical AND with immediate
434: Error: Invalid line
+000000EB:   6B08        ORI     R16,0xB8         Logical OR with immediate
435: Error: Invalid line
+000000EC:   BB05        OUT     0x15,R16         Out to I/O location
436: Error: Invalid line
+000000ED:   DFD4        RCALL   PC-0x002B        Relative call subroutine
437: Error: Invalid line
+000000EE:   9508        RET                      Subroutine return
@000000EF: SET_column
440: Error: Invalid line
+000000EF:   DFCF        RCALL   PC-0x0030        Relative call subroutine
441: Error: Invalid line
+000000F0:   2F06        MOV     R16,R22          Copy register
442: Error: Invalid line
+000000F1:   FF06        SBRS    R16,6            Skip if bit in register set
443: Error: Invalid line
+000000F2:   DFC9        RCALL   PC-0x0036        Relative call subroutine
444: Error: Invalid line
+000000F3:   730F        ANDI    R16,0x3F         Logical AND with immediate
445: Error: Invalid line
+000000F4:   6400        ORI     R16,0x40         Logical OR with immediate
446: Error: Invalid line
+000000F5:   BB05        OUT     0x15,R16         Out to I/O location
447: Error: Invalid line
+000000F6:   DFCB        RCALL   PC-0x0034        Relative call subroutine
448: Error: Invalid line
+000000F7:   9508        RET                      Subroutine return
@000000F8: OSWRCH
453: Error: Invalid line
+000000F8:   FD76        SBRC    R23,6            Skip if bit in register cleared
454: Error: Invalid line
+000000F9:   C01B        RJMP    PC+0x001C        Relative jump
456: Error: Invalid line
+000000FA:   FD71        SBRC    R23,1            Skip if bit in register cleared
457: Error: Invalid line
+000000FB:   D1D9        RCALL   PC+0x01DA        Relative call subroutine
458: Error: Invalid line
+000000FC:   DFDF        RCALL   PC-0x0020        Relative call subroutine
460: Error: Invalid line
+000000FD:   9180008E    LDS     R24,0x008E       Load direct from data space
462: Error: Invalid line
+000000FF:   D1B4        RCALL   PC+0x01B5        Relative call subroutine
464: Error: Invalid line
+00000100:   2D08        MOV     R16,R8           Copy register
@00000101: NEXT_SLICE
466: Error: Invalid line
+00000101:   9115        LPM     R17,Z+           Load program memory and postincrement
467: Error: Invalid line
+00000102:   2718        EOR     R17,R24          Exclusive OR
468: Error: Invalid line
+00000103:   BB15        OUT     0x15,R17         Out to I/O location
469: Error: Invalid line
+00000104:   DFBD        RCALL   PC-0x0042        Relative call subroutine
470: Error: Invalid line
+00000105:   3130        CPI     R19,0x10         Compare with immediate
471: Error: Invalid line
+00000106:   F419        BRNE    PC+0x04          Branch if not equal
472: Error: Invalid line
+00000107:   BB15        OUT     0x15,R17         Out to I/O location
473: Error: Invalid line
+00000108:   DFB9        RCALL   PC-0x0046        Relative call subroutine
474: Error: Invalid line
+00000109:   950A        DEC     R16              Decrement
@0000010A: NORMAL_CHARS
476: Error: Invalid line
+0000010A:   950A        DEC     R16              Decrement
477: Error: Invalid line
+0000010B:   F7A9        BRNE    PC-0x0A          Branch if not equal
479: Error: Invalid line
+0000010C:   0D68        ADD     R22,R8           Add without carry
480: Error: Invalid line
+0000010D:   156B        CP      R22,R11          Compare
481: Error: Invalid line
+0000010E:   F028        BRCS    PC+0x06          Branch if carry set
482: Error: Invalid line
+0000010F:   2D6A        MOV     R22,R10          Copy register
483: Error: Invalid line
+00000110:   9553        INC     R21              Increment
484: Error: Invalid line
+00000111:   155D        CP      R21,R13          Compare
485: Error: Invalid line
+00000112:   F409        BRNE    PC+0x02          Branch if not equal
486: Error: Invalid line
+00000113:   C1BC        RJMP    PC+0x01BD        Relative jump
@00000114: OSWRCH_OUT
488: Error: Invalid line
+00000114:   9508        RET                      Subroutine return
@00000115: GRAPHIC_CHAR_OUT
493: Error: Invalid line
+00000115:   2733        CLR     R19              Clear Register
494: Error: Invalid line
+00000116:   D060        RCALL   PC+0x0061        Relative call subroutine
495: Error: Invalid line
+00000117:   3F3F        CPI     R19,0xFF         Compare with immediate
496: Error: Invalid line
+00000118:   F409        BRNE    PC+0x02          Branch if not equal
497: Error: Invalid line
+00000119:   9508        RET                      Subroutine return
@0000011A: SKIP_34
500: Error: Invalid line
+0000011A:   935F        PUSH    R21              Push register on stack
501: Error: Invalid line
+0000011B:   E6E8        LDI     R30,0x68         Load immediate
502: Error: Invalid line
+0000011C:   E0F0        LDI     R31,0x00         Load immediate
503: Error: Invalid line
+0000011D:   940E0AA2    CALL    0x00000AA2       Call subroutine
505: Error: Invalid line
+0000011F:   D194        RCALL   PC+0x0195        Relative call subroutine
507: Error: Invalid line
+00000120:   2D38        MOV     R19,R8           Copy register
508: Error: Invalid line
+00000121:   2D58        MOV     R21,R8           Copy register
@00000122: NEXT_GRAPHIC_SLICE
510: Error: Invalid line
+00000122:   9145        LPM     R20,Z+           Load program memory and postincrement
512: Error: Invalid line
+00000123:   3130        CPI     R19,0x10         Compare with immediate
513: Error: Invalid line
+00000124:   F429        BRNE    PC+0x06          Branch if not equal
514: Error: Invalid line
+00000125:   D035        RCALL   PC+0x0036        Relative call subroutine
515: Error: Invalid line
+00000126:   9503        INC     R16              Increment
516: Error: Invalid line
+00000127:   F409        BRNE    PC+0x02          Branch if not equal
517: Error: Invalid line
+00000128:   9513        INC     R17              Increment
@00000129: SKIP_12
519: Error: Invalid line
+00000129:   955A        DEC     R21              Decrement
@0000012A: SKIP_64
521: Error: Invalid line
+0000012A:   D030        RCALL   PC+0x0031        Relative call subroutine
522: Error: Invalid line
+0000012B:   9503        INC     R16              Increment
523: Error: Invalid line
+0000012C:   F409        BRNE    PC+0x02          Branch if not equal
524: Error: Invalid line
+0000012D:   9513        INC     R17              Increment
@0000012E: SKIP_14
526: Error: Invalid line
+0000012E:   955A        DEC     R21              Decrement
527: Error: Invalid line
+0000012F:   F791        BRNE    PC-0x0D          Branch if not equal
529: Error: Invalid line
+00000130:   915F        POP     R21              Pop register from stack
530: Error: Invalid line
+00000131:   D067        RCALL   PC+0x0068        Relative call subroutine
531: Error: Invalid line
+00000132:   9508        RET                      Subroutine return
@00000133: GRAPHIC_USER_DEF
534: Error: Invalid line
+00000133:   2733        CLR     R19              Clear Register
535: Error: Invalid line
+00000134:   D042        RCALL   PC+0x0043        Relative call subroutine
536: Error: Invalid line
+00000135:   3F3F        CPI     R19,0xFF         Compare with immediate
537: Error: Invalid line
+00000136:   F409        BRNE    PC+0x02          Branch if not equal
538: Error: Invalid line
+00000137:   9508        RET                      Subroutine return
@00000138: SKIP_35
541: Error: Invalid line
+00000138:   935F        PUSH    R21              Push register on stack
542: Error: Invalid line
+00000139:   E6E8        LDI     R30,0x68         Load immediate
543: Error: Invalid line
+0000013A:   E0F0        LDI     R31,0x00         Load immediate
544: Error: Invalid line
+0000013B:   940E0AA2    CALL    0x00000AA2       Call subroutine
546: Error: Invalid line
+0000013D:   2D38        MOV     R19,R8           Copy register
547: Error: Invalid line
+0000013E:   DF65        RCALL   PC-0x009A        Relative call subroutine
548: Error: Invalid line
+0000013F:   714F        ANDI    R20,0x1F         Logical AND with immediate
549: Error: Invalid line
+00000140:   0F44        LSL     R20              Logical Shift Left
550: Error: Invalid line
+00000141:   0F44        LSL     R20              Logical Shift Left
551: Error: Invalid line
+00000142:   0F44        LSL     R20              Logical Shift Left
552: Error: Invalid line
+00000143:   EEC8        LDI     R28,0xE8         Load immediate
553: Error: Invalid line
+00000144:   E0D0        LDI     R29,0x00         Load immediate
554: Error: Invalid line
+00000145:   0FC4        ADD     R28,R20          Add without carry
555: Error: Invalid line
+00000146:   F408        BRCC    PC+0x02          Branch if carry cleared
556: Error: Invalid line
+00000147:   95D3        INC     R29              Increment
@00000148: SKIP_25
559: Error: Invalid line
+00000148:   2D38        MOV     R19,R8           Copy register
560: Error: Invalid line
+00000149:   2D58        MOV     R21,R8           Copy register
@0000014A: NEXT_USER_GRAPHIC_SLICE
562: Error: Invalid line
+0000014A:   9149        LD      R20,Y+           Load indirect and postincrement
563: Error: Invalid line
+0000014B:   3130        CPI     R19,0x10         Compare with immediate
564: Error: Invalid line
+0000014C:   F429        BRNE    PC+0x06          Branch if not equal
565: Error: Invalid line
+0000014D:   D00D        RCALL   PC+0x000E        Relative call subroutine
566: Error: Invalid line
+0000014E:   9503        INC     R16              Increment
567: Error: Invalid line
+0000014F:   F409        BRNE    PC+0x02          Branch if not equal
568: Error: Invalid line
+00000150:   9513        INC     R17              Increment
@00000151: SKIP_28
570: Error: Invalid line
+00000151:   955A        DEC     R21              Decrement
@00000152: SKIP_29
572: Error: Invalid line
+00000152:   D008        RCALL   PC+0x0009        Relative call subroutine
573: Error: Invalid line
+00000153:   9503        INC     R16              Increment
574: Error: Invalid line
+00000154:   F409        BRNE    PC+0x02          Branch if not equal
575: Error: Invalid line
+00000155:   9513        INC     R17              Increment
@00000156: SKIP_32
577: Error: Invalid line
+00000156:   955A        DEC     R21              Decrement
578: Error: Invalid line
+00000157:   F791        BRNE    PC-0x0D          Branch if not equal
580: Error: Invalid line
+00000158:   915F        POP     R21              Pop register from stack
581: Error: Invalid line
+00000159:   D03F        RCALL   PC+0x0040        Relative call subroutine
583: Error: Invalid line
+0000015A:   C252        RJMP    PC+0x0253        Relative jump
@0000015B: OUTPUT_GRAPHIC_SLICE
590: Error: Invalid line
+0000015B:   935F        PUSH    R21              Push register on stack
591: Error: Invalid line
+0000015C:   933F        PUSH    R19              Push register on stack
592: Error: Invalid line
+0000015D:   934F        PUSH    R20              Push register on stack
593: Error: Invalid line
+0000015E:   938F        PUSH    R24              Push register on stack
594: Error: Invalid line
+0000015F:   939F        PUSH    R25              Push register on stack
596: Error: Invalid line
+00000160:   E058        LDI     R21,0x08         Load immediate
@00000161: OUTPUT_GRAPHIC_DOT
598: Error: Invalid line
+00000161:   9547        ROR     R20              Rotate right through carry
599: Error: Invalid line
+00000162:   F448        BRCC    PC+0x0A          Branch if carry cleared
601: Error: Invalid line
+00000163:   940E0AA8    CALL    0x00000AA8       Call subroutine
602: Error: Invalid line
+00000165:   F431        BRNE    PC+0x07          Branch if not equal
604: Error: Invalid line
+00000166:   940E0A88    CALL    0x00000A88       Call subroutine
606: Error: Invalid line
+00000168:   902000B8    LDS     R2,0x00B8        Load direct from data space
608: Error: Invalid line
+0000016A:   940E0A31    CALL    0x00000A31       Call subroutine
@0000016C: NO_DOT_TO_PLOT
611: Error: Invalid line
+0000016C:   958A        DEC     R24              Decrement
612: Error: Invalid line
+0000016D:   F409        BRNE    PC+0x02          Branch if not equal
613: Error: Invalid line
+0000016E:   959A        DEC     R25              Decrement
@0000016F: SKIP_11
615: Error: Invalid line
+0000016F:   955A        DEC     R21              Decrement
616: Error: Invalid line
+00000170:   F781        BRNE    PC-0x0F          Branch if not equal
618: Error: Invalid line
+00000171:   919F        POP     R25              Pop register from stack
619: Error: Invalid line
+00000172:   918F        POP     R24              Pop register from stack
620: Error: Invalid line
+00000173:   914F        POP     R20              Pop register from stack
621: Error: Invalid line
+00000174:   913F        POP     R19              Pop register from stack
622: Error: Invalid line
+00000175:   915F        POP     R21              Pop register from stack
623: Error: Invalid line
+00000176:   9508        RET                      Subroutine return
@00000177: GRAPHIC_CHARACTER_MODE
626: Error: Invalid line
+00000177:   930F        PUSH    R16              Push register on stack
627: Error: Invalid line
+00000178:   931F        PUSH    R17              Push register on stack
628: Error: Invalid line
+00000179:   91100081    LDS     R17,0x0081       Load direct from data space
629: Error: Invalid line
+0000017B:   91000080    LDS     R16,0x0080       Load direct from data space
631: Error: Invalid line
+0000017D:   2300        TST     R16              Test for Zero or Minus
632: Error: Invalid line
+0000017E:   F441        BRNE    PC+0x09          Branch if not equal
634: Error: Invalid line
+0000017F:   3010        CPI     R17,0x00         Compare with immediate
