sdcc -mz80 --opt-code-speed --code-loc 0x001A --data-loc 0 --no-std-crt0 crt0.rel bvideo.c
..\hex2bin bvideo.ihx
del bvideo
ren bvideo.bin bvideo