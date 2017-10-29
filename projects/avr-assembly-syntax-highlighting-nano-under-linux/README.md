# AVR assembly: Syntax Highlighting in nano under Linux

Uploaded by lubel on 2017-10-29 11:37:00 (rating 5 out of 5)

## Summary

 


## Syntaxhighlighting for AVR Assembly (Atmel instruction set)  

## (c) by Lutz Bellmann, 2017


## Place in /usr/share/nano ; replace asm.nanorc; make backup of original asm.nanorc


syntax "asm" "\.(S|s|asm)$"  

magic "[Aa]ssembl(y|er)"


# Befehle  

icolor green "^(clr|ser|ldi|mov|lds|ld|ldd|in|pop|lpm|sts|st|std|out|push|inc|add|adiw|dec|sub|subi|sbc|sbci|sbiw|lsl|lsr|rol|ror|asr|swap|and|andi|or|ori|eor|com|neg|sbr|cbr|bst|bld|sbi|cbi|sez|sec|sen|sev|seh|ses|set|sei|clz|clc|cln|clt|cli|cp|cpc|cpi|tst|rjmp|ijmp|rcall|ret|reti|brbs||brbc|breq|brne|brcs|brcc|brsh|brlo|brmi|brpl|brge|brlt|brhs|brhc|brts|brtc|brvs|brvc|brie|brid|sbrc|sbrs|sbic|sbis|cpse|nop|sleep|wdr)*"


# Sprungpunkte und Rountinen  

color brightred "^[[:alnum:]\_]+:$"


# Assemblerdirektiven  

icolor brightgreen "\.(cseg|db|def|dw|endmacro|eseg|equ|include|macro|org)"


# Binär- und Hexzahlen


icolor brightblue "(0b[0-1]{8}|0x[a-fA-F0-9]{2,4}|RAMEND|HIGH|LOW)"


# Bitoperationen


icolor blue "1<<[[:alnum:]]*"


# Strings.  

icolor brightyellow "<[^=     ]*>" ""(\\.|[^"])*""


# Comments.  

color cyan ";.*"


# Trailing whitespace.  

# color ,green "[[:space:]]+$"

## Tags

- Complete code
- #AVR_STUDIO #LINUX #IDE #NANO #ASSEMBLY #SYNTAX_HIGHLIGHTING
