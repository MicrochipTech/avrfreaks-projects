;; FILE: crt0.s
;; Generic crt0.s for a Z80
;; From SDCC..
;; Modified to suit execution on the Amstrad CPC!
;; by H. Hansen 2003
;; Original lines has been marked out!

  .module crt0
  .globl  _main

  .area _HEADER (ABS)
;; Reset vector
  .org  0x0008
  jp  init
  
  .org  0x000C

init:

;; Stack at the top of memory.
;;  ld  sp,#0x7fff        
;;  I will use the Basic stack, so the program can return to basic!

;; Initialise global variables
  call    gsinit
  call  _main
  jp  _exit

  ;; Ordering of segments for the linker.
  .area _HOME
  .area _CODE
  .area   _GSINIT
  .area   _GSFINAL
        
  .area _DATA
  .area   _BSS
  .area   _HEAP

  .area   _CODE
__clock::
  ret
  
_exit::
    ld      iy,#0
    ld      (iy), #0xFF
    ld      1 (iy), l
    call    0x0007
  ret
  
  .area   _GSINIT
gsinit::  

.area   _GSFINAL
  ret