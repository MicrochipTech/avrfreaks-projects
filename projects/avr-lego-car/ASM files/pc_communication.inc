.equ call_turn = 1
.equ call_move_distance = 2
.equ call_move_hut = 3
.equ call_start_hd_adc = 4
.equ call_init_hut = 5
.equ call_start_fast_adc = 6
.equ call_start_moving = 7
.equ call_stop_moving = 8

pc_control_loop:
call RECEIVE_BYTE
cpi usart_byte,call_turn
brne pc+2
jmp call_turn_rout
cpi usart_byte,call_move_distance
brne pc+2
jmp call_move_distance_rout
cpi usart_byte,call_move_hut
brne pc+2
jmp call_move_hut_rout
cpi usart_byte,call_start_hd_adc
brne pc+2
jmp call_start_hd_adc_rout
cpi usart_byte,call_init_hut
brne pc+2
jmp call_init_hut_rout
cpi usart_byte,call_start_fast_adc
brne pc+2
jmp call_start_fast_adc_rout
cpi usart_byte,call_start_moving
brne pc+2
jmp call_start_moving_rout
cpi usart_byte,call_stop_moving
brne pc+2
jmp call_stop_moving_rout
rjmp pc_control_loop


call_turn_rout:
call RECEIVE_BYTE
mov steering,usart_byte
call TURN
jmp pc_control_loop

call_move_distance_rout:
call RECEIVE_BYTE
mov speed,usart_byte
call RECEIVE_BYTE
mov distance,usart_byte
call RECEIVE_BYTE
mov direction,usart_byte
call MOVE_DISTANCE
jmp pc_control_loop 

call_move_hut_rout:
call RECEIVE_BYTE
mov angle,usart_byte
call MOVE_HUT
jmp pc_control_loop 

call_start_hd_adc_rout:
call START_HD_ADC_1_BY_VCC
in usart_byte,ADCH
call TRANSMITE_BYTE
jmp pc_control_loop 

call_init_hut_rout:
call init_hut
jmp pc_control_loop 

call_start_fast_adc_rout:
call START_FAST_ADC_1_BY_VCC
in usart_byte,ADCH
call TRANSMITE_BYTE
jmp pc_control_loop 

call_start_moving_rout:
call RECEIVE_BYTE
mov speed,usart_byte
call RECEIVE_BYTE
mov direction,usart_byte
call START_MOVING
jmp pc_control_loop 

call_stop_moving_rout:
call STOP_MOVING
jmp pc_control_loop 
