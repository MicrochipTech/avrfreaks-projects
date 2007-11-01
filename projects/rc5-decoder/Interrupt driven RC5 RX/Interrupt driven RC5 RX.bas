'-------------------------------------------------------------------------------
' (c) 2007 Toon (bbubble62@hotmail.com))
'
' program partially ported from C-code for PIC processor
'
'-------------------------------------------------------------------------------
$regfile = "m8def.dat"
$crystal = 8000000
$baud = 57600
'
'stack space
'
$hwstack = 50
$swstack = 32
$framesize = 50
'
'reserve space for variables
'
dim RC5Available as bit                                     'set when RC5 data is available
dim RC5Command as byte                                      'contains RC5 command
dim RC5Address as byte                                      'contains RC5 Address
dim RC5Data as word                                         'temp var used to store data when acquiring RC5 code
dim RC5BitCount as byte                                     'temp var used to count incoming bits
dim RC5PhaseError as bit                                    'temp var use to indicate if a phase error occured
dim RC5ControlBit as bit                                    'temp var used to store previous control bit
dim RC5Repeat as bit                                        'set when data is repeated
dim RC5OldValue as bit                                      'temp var use to hold bit phase
'
dim Tmp1 as byte                                            'var temp
'
'magic numbers for reload ...
'freq = 8 MHz = 125 nS clockcycle
'bittime RC5 = 889 uS x 8  = 7112 clockcycles
'time from first neg. edge to middle of first half control bit = 3.5 * 7112 = 24892 cycles
'--> Timer_Reload0 = 65536 - 24892 = 40644 + interrupt latency = ~ 65 cycles
'time for bit-to-bit = 1 x 7112 = 7112 cycles
'--> Timer_Reload1 = 65536 - 7112 = 58424 + interrupt latency = ~ 65 cycles

Const Timer1_Reload0 = 40644 + 65                           'used for time to first bit
Const Timer1_Reload1 = 58424 + 65                           'time from 1st to 2nd ((etc) bit

Config WATCHDOG = 1024                                      'watchdog setting at ~ 1 second
'
TestPin alias PORTC.5                                       'for debugging purposes
Config TestPin = output                                     'make it output
'
Pin_IR alias PIND.2                                         'RC5 hardware goes here
Config Pin_IR = Input                                       'make it input
Set POrtD.2                                                 'set pull resistor
'
Config Int0 = Falling                                       'falling edge
On Int0 ISR_Int0                                            'interrupt service routine INT0
Enable Int0                                                 'enable interrupt 0                                              'enable interrupt timer0

Config Timer1 = Timer , prescale = 1                        'timer1
On Timer1 ISR_TMR1                                          'interrupt service routine TIMER1
Stop TIMER1                                                 'stop it
Enable Timer1                                               'enable interrupt timer1
'
Enable Interrupts                                           'enable all interrupts
'
Waitms 250                                                  'some delay
Print "Welcome to RC5Decode !"                              'welcome !

Reset RC5Available                                          'no rc5 available yet

Start watchdog
'
do
   Reset watchdog                                           'in case of problems
   if RC5Available = 1 then
      print "RC5Address: " ; RC5Address ; ", RC5Command  : " ; RC5Command ; ", RC5Repeat : " ; RC5Repeat ; ", RC5ControlBit :" ; RC5ControlBit
      reset RC5Available
   end if
loop                                                        'main loop

end

ISR_Int0:                                                   'executed at first pos/neg transition of RC5 code

  Timer1 = Timer1_Reload0                                   'set timer1 to first reload
  Start TIMER1                                              'start timer1
  Disable INt0                                              'disable INT0
  RC5BitCount = 0                                           'no bits reeived yet
return

ISR_TMR1:
                                                  'RC5 code interrupt
   Timer1 = Timer1_Reload1                                  'reload value for next bit
   if RC5BitCount = 0 then                                  'first time
      RC5Data = 0                                           'No data yet
      RC5OldValue = Pin_IR                                  ' get status
      RC5PhaseError = 0                                     ' no phase error yet
   endif

   if RC5BitCount < 24 then                                 'as long as less then 25 bits
      Tmp1 = RC5BitCount AND 1                              ' see if bitcount = odd/even
      if Tmp1 = 1 then                                      'odd
         If RC5OldValue <> Pin_IR Then                      'is it same as Bit old data ?
            shift RC5Data , left , 1                        'shift in 1
            If Pin_IR = 1 then                              'if input = 1
               RC5Data = RC5Data AND &HFFFE                 'make result low bit --> low
            else
               RC5Data = RC5Data OR &H0001                  'make result low bit --> high
            endif
         else
            RC5PhaseError = 1                               'if not then phase error
         endif
      endif
      Incr RC5BitCount
      RC5OldValue = Pin_IR
   else                                                     'we received 24 bits
      Stop Timer1                                           'stop timer
      If RC5PhaseError = 0 then                             ' if no phase error
         RC5Command = RC5Data AND &H003F                    ' get Command
         shift RC5Data , right , 6                          'shift address
         RC5Address = RC5Data AND &H001F                    'get address
         Shift RC5Data , right , 5                          'shift controlbit
         RC5Data = RC5Data AND &H0001                       'get controlbit
         RC5Repeat = RC5Controlbit XOR RC5Data
         Toggle RC5Repeat                                   'set repeatbit if needed
         RC5Controlbit = RC5Data                            'store controlbit for next bitstream
         RC5Available = 1                                   'data is availabe !!
      endif
      RC5BitCount = 0
      Set gifr.INTF0                                        'clear pending INT0 interrupt caused by previous falling edges
      enable INt0                                           'enable INT0 again
   endif
return
