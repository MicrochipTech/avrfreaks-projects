;; FILE: putchar.s
;; Modified to suit execution on the Amstrad CPC
;; by H. Hansen 2003
;; Original lines has been marked out!

  .area _CODE
_putchar::       
_putchar_rr_s:: 
          ld      hl,#2
          add     hl,sp
        
          ld      a,(hl)
          ld      iy, #0
          ld      (iy), #1
          ld      1 (iy), a
          call    0x0007
          ret
           
_putchar_rr_dbs::

          ld      iy, #0
          ld      (iy), #1
          ld      1 (iy), e
          call    0x0007
          ret