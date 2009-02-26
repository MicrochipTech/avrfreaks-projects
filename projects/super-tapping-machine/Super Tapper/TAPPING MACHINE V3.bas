$regfile = "m8def.dat"
$crystal = 1000000
$baud = 9600

Config Portd = &B00100000                                   'PD7=top but, PD6=bot but, PD5=light, PD4=Encoder
Portd = &B11100111                                          'PD3=Encoder, PD2=Speed sense, PD1=TXD, PD0=RXD
Config Portb = &B00000111                                   'PB0=CW, PB1=CCW, PB2=Enable
Config Pinc.4 = Output
Config Adc = Single , Prescaler = Auto
Start Adc
Config Timer1 = Pwm , Pwm = 8 , Prescale = 8 , Compare A Pwm = Clear Down       'motor
Enable Compare1a
Start Timer1
Config Int0 = Falling
On Int0 Update_speed_counter
Enable Int0
Config Timer2 = Timer , Prescale = 1024
On Ovf2 Update_speed
Enable Timer2
Enable Ovf2
Start Timer2
Config Timer0 = Timer , Prescale = 1
On Ovf0 Update_depth
Start Timer0
Enable Timer0
Enable Interrupts



Light Alias Portd.5
En Alias Portb.1
Cw Alias Portb.0
Ccw Alias Portb.2
Top_button Alias Pind.7
Bottom_button Alias Pind.6
Air Alias Portc.4
Get_current Alias Getadc(2)
Get_current_limit Alias Getadc(1)
Get_target_speed Alias Getadc(0)

Dim Speed_delay As Byte
Dim Speed_counter As Word
Dim Speed_counter1 As Word
Dim Speed As Word
Dim Target_speed As Word
Dim Speed_adjustment As Integer
Dim Current As Word
Dim Current_limit As Word
Dim Duty As Integer
Dim Current_encoder_value As Byte
Dim Previous_encoder_value As Byte
Dim Depth As Integer
Dim Starting As Word
Dim Clear As Word
Dim Bottom As Word
Dim Dir As Boolean
Dim Minus_flag As Boolean

Const Forward = 1
Const Backward = 0

Starting = 10
Clear = 70
Bottom = 270





'Program begins!
Depth = 50                                                  'fool machine into going back
Gosub Go_backwards
Target_speed = 100
Do
Loop Until Speed_counter1 = 500                             'well and truely out of hole
Depth = 0
Gosub Go_forwards

Do
   Target_speed = Get_target_speed / 8
   If Depth = 0 Then Target_speed = 0
   If Depth > Bottom Then
      If Dir = Forward Then
         Gosub Brake
         Gosub Go_backwards
      End If
   End If
   If Depth < Clear Then
         If Dir = Backward Then
            Gosub Blast
            Gosub Brake
            Gosub Go_forwards
         End If
   End If
   Current_limit = Get_current_limit / 2
   Current = Get_current
   If Current > Current_limit Then
      Gosub Brake
      Gosub Go_backwards
      Target_speed = 50
      Speed_counter1 = 0
      Do
      Loop Until Speed_counter1 = 500
      Gosub Brake
      Gosub Go_forwards
   End If
   Print "Target_speed " ; Target_speed ; " Speed " ; Speed ; " Duty " ; Duty ; " Current Limit " ; Current_limit ; " Current Sense " ; Get_current ; " Depth " ; Depth
Loop






Update_speed_counter:
   Incr Speed_counter
   Incr Speed_counter1
Return

Update_speed:
   Incr Speed_delay
   If Speed_delay = 15 Then                                 '15 x ovf2 is approprate interval to read speed
      Speed_delay = 0
      Speed = Speed_counter
      Speed_counter = 0
      Minus_flag = 0
      Speed_adjustment = Target_speed - Speed
      If Speed_adjustment < 0 Then Minus_flag = 1
      Speed_adjustment = Speed_adjustment * Speed_adjustment
      Speed_adjustment = Speed_adjustment / 8
      If Minus_flag = 1 Then
         Duty = Duty - Speed_adjustment
      Else
         Duty = Duty + Speed_adjustment
      End If
      If Duty > 255 Then Duty = 255
      If Duty < 0 Then Duty = 0
      If Target_speed = 0 Then Duty = 0
      If Depth < Starting Then Duty = 0
      Compare1a = Duty
      Toggle Light
   End If
Return

Update_depth:
    Current_encoder_value = Pind And 24
    Select Case Current_encoder_value
     Case 0
     Select Case Previous_encoder_value
         Case 16
            Incr Depth
         Case 8
            Decr Depth
         Case 24
            Depth = 0
     End Select
     Case 16
        If Previous_encoder_value = 0 Then Decr Depth
     Case 8
        If Previous_encoder_value = 0 Then Incr Depth
    End Select
    Previous_encoder_value = Current_encoder_value
    If Depth < 0 Then Depth = 0
Return

Go_forwards:
   Set Cw
   Reset Ccw
   Dir = Forward
Return

Go_backwards:
   Set Ccw
   Reset Cw
   Dir = Backward
Return

Blast:
   Set Air
   Waitms 100
   Reset Air
Return

Brake:
   Target_speed = 0
   Duty = 0
   Compare1a = 0
   Reset Cw
   Reset Ccw
Return