.include "m16def.inc"
.include "intelligent_car_defs.inc"
.include "wheel_routines.inc"
.include "steering_routines.inc"
.include "hut_routines.inc"
.include "adc_routines.inc"
.include "time_delay_routines.inc"
.include "usart_routines.inc"
.include "pc_communication.inc"

MAIN:

sbic pind,0x04		;An to jack einai mesa
jmp pc_control_loop	;pame sti routina elegxou apo PC

ldi speed,230		;Orizw tahytita
ldi distance,1		;Kai apostasi pou tha dianiw kathe fora

main_loop:

call init_hut
ldi angle,1			;Sarwnw aristerostrofa ton horo
clr max_light		
call move_hut		
call choose_move	;Apofasizw gia tin kinisi
ldi angle,47		;Sarwnw dexiostrofa ton horo
clr max_light
call move_hut
call choose_move	;Apofasizw gia tin kinisi

jmp main_loop

choose_move:		
cpi max_angle,33	;An to megisto fws einai pisw kai dexia
brlo resume1
ldi steering,0		;kanw aristera kai 
call turn
ldi direction,rear	;opisthen (manouvra)
call move_distance
ret
resume1:
cpi max_angle,26	;An mprosta dexia
brlo resume2
ldi steering,255	;kanw dexia kai 
call turn
ldi direction,front	;mpros
call move_distance
ret
resume2:			;an mprosta 
cpi max_angle,22
brlo resume3
ldi steering,128	;isiwnw kremariera
call turn
ldi direction,front	;paw mprosta
call move_distance
ret
resume3:
cpi max_angle,14	;An mprosta aristera
brlo resume4
ldi steering,0		;paw aristera
call turn
ldi direction,front	;kai mprosta
call move_distance
ret
resume4:
ldi steering,255	;Kanw dexia kai
call turn
ldi direction,rear	;pisw (manouvra)
call move_distance

ret
