# Delay LOOPs

Uploaded by maaply on 2008-07-25 09:39:00 (rating 0 out of 5)

## Summary

This code i tested with AVR Studio and simulator.  

I used this code for many projects. It is working fine. this is very useful to understand the interface modules exact requirements.


<<< maaply >>>


; ************************************************************************************


dly\_10cy: nop  

 nop  

 nop  

 ret


dly\_20cy: push r17  

 ldi r17,0x03  

dly\_20cy\_l1: dec r17  

 brne dly\_20cy\_l1  

 pop r17  

 ret


dly\_50cy: push r17  

 ldi r17,0x0D  

dly\_50cy\_l1: dec r17  

 brne dly\_50cy\_l1  

 pop r17  

 ret 


dly\_100cy: push r17  

 ldi r17,0x1D  

dly\_100cy\_l1: dec r17  

 brne dly\_100cy\_l1  

 nop  

 nop  

 pop r17  

 ret 


dly\_500cy: push r17  

 ldi r17,0xA3  

dly\_500cy\_l1: dec r17  

 brne dly\_500cy\_l1  

 pop r17  

 ret


dly\_1000cy: push r17  

 ldi r17,0xFF  

dly\_1000cy\_l1: dec r17  

 brne dly\_1000cy\_l1  

 ldi r17,0x4A  

dly\_1000cy\_l2: dec r17  

 brne dly\_1000cy\_l2  

 nop  

 nop  

 pop r17  

 ret


dly\_5000cy: push r17  

 push r18  

 ldi r17,0xE9  

dly\_5000cy\_l2: ldi r18,0x06  

dly\_5000cy\_l1: dec r18  

 brne dly\_5000cy\_l1  

 dec r17  

 brne dly\_5000cy\_l2  

 ldi r18,0x17  

dly\_5000cy\_l3: dec r18  

 nop  

 brne dly\_5000cy\_l3  

 pop r18  

 pop r17  

 ret


dly\_10000cy: push r17  

 push r18  

 ldi r17,0xED  

dly\_10000cy\_l2: ldi r18,0x0D  

dly\_10000cy\_l1: dec r18  

 brne dly\_10000cy\_l1  

 dec r17  

 brne dly\_10000cy\_l2  

 ldi r18,0x0A  

dly\_10000cy\_l3: dec r18  

 brne dly\_10000cy\_l3  

 nop  

 pop r18  

 pop r17  

 ret


dly\_50000cy: push r17  

 push r18  

 ldi r17,0xFF  

dly\_50000cy\_l2: ldi r18,0x40  

dly\_50000cy\_l1: dec r18  

 brne dly\_50000cy\_l1  

 dec r17  

 brne dly\_50000cy\_l2  

 ldi r18,0x41  

dly\_50000cy\_l3: dec r18  

 nop  

 brne dly\_5000cy\_l3  

 pop r18  

 pop r17  

 ret


dly\_100000cy: push r17  

 push r18  

 ldi r17,0xFF  

dly\_100000cy\_l2: ldi r18,0x81  

dly\_100000cy\_l1: dec r18  

 brne dly\_100000cy\_l1  

 dec r17  

 brne dly\_100000cy\_l2  

 ldi r18,0xB1  

dly\_100000cy\_l3: dec r15  

 brne dly\_10000cy\_l3  

 pop r18  

 pop r17  

 ret 


dly\_1mcy: push r17  

 push r18  

 push r19  

 ldi r17,0x90  

dly\_1mcy\_l3: ldi r18,0xEA  

dly\_1mcy\_l2: ldi r19,07  

dly\_1mcy\_l1: dec r19  

 brne dly\_1mcy\_l1  

 dec r18  

 brne dly\_1mcy\_l2  

 dec r17  

 brne dly\_1mcy\_l3  

 ldi r17,0xF9  

dly\_1mcy\_l5: ldi r18,0xFE  

dly\_1mcy\_l4: dec r18  

 brne dly\_1mcy\_l4  

 nop  

 dec r17  

 brne dly\_1mcy\_l5  

 ldi r17,0x25  

dly\_1mcy\_l6: dec r17  

 brne dly\_1mcy\_l6  

 pop r19  

 pop r18  

 pop r17  

 ret 


dly\_5mcy: push r17  

 push r18  

 push r19  

 ldi r17,0xFF  

dly\_5mcy\_l3: ldi r18,0xC5  

dly\_5mcy\_l2: ldi r19,0x18  

dly\_5mcy\_l1: nop  

 dec r19  

 brne dly\_5mcy\_l1  

 dec r18  

 brne dly\_5mcy\_l2  

 nop  

 nop  

 dec r17  

 brne dly\_5mcy\_l3  

 ldi r17,0xFF  

dly\_5mcy\_l5: ldi r18,0x18  

dly\_5mcy\_l4: nop  

 dec r18  

 brne dly\_5mcy\_l4  

 dec r17  

 brne dly\_5mcy\_l5  

 ldi r17,0x41  

dly\_5mcy\_l6: dec r17  

 brne dly\_5mcy\_l6  

 nop  

 pop r19  

 pop r18  

 pop r17  

 ret


dly\_10mcy: push r17  

 push r18  

 push r19  

 ldi r17,0xFE  

dly\_10mcy\_l3: ldi r18,0xFC  

dly\_10mcy\_l2: ldi r19,0x33  

dly\_10mcy\_l1: dec r19  

 brne dly\_10mcy\_l1  

 dec r18  

 brne dly\_10mcy\_l2  

 dec r17  

 brne dly\_10mcy\_l3  

 ldi r17,0xFF  

dly\_10mcy\_l5: ldi r18,0x10  

dly\_10mcy\_l4: dec r18  

 brne dly\_10mcy\_l4  

 dec r17  

 brne dly\_10mcy\_l5  

 ldi r17,0xF1  

dly\_10mcy\_l6: nop  

 dec r17  

 brne dly\_10mcy\_l6  

 nop  

 nop  

 pop r19  

 pop r18  

 pop r17  

 ret

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
