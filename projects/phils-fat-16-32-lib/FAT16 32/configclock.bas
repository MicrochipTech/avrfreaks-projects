'Configuration for Clock

Config Date = Dmy , Separator = /                           'How do you like your date?
Const Clock_is_ds1307 = 1                                   'Type of clock is?
#if Clock_is_ds1307 = 1
'Hardware Clock
   Config Clock = User
   $lib "ds1307rtc.lbx"
   $external Getdatetime , Setdate , Settime
   $lib "i2c_TWI.lbx"
   Config Twi = 400000                                      '400k
   I2cinit                                                  'Init the TWI
   #if Media_is_rotating = 1
'hardware Clock & sectic to keep track of goto sleep & timeouts
      'For chips with async timer 0 eg MEGA128
      Config Timer0 = Timer , Async = On , Prescale = 128   '1 second interupt
      On Timer0 Sectic Nosave                               'where to go every second
      Enable Timer0                                         'enable interupt
      'For chips with async timer 2 eg MEGA162
      'Config Timer2 = Timer , Async = On , Prescale = 128   '1 second interupt
      'On Timer2 Sectic Nosave                               'where to go every second
      'Enable Timer2                                         'enable interupt
   #endif
'hardware clock only
#else
'Software Clock
   #if Media_is_rotating = 1
'Software Clock & sectic to keep track of goto sleep & timeouts
      Config Clock = Soft , Gosub = Sectic
   #else
'Non rotating media & software clock
      Config Clock = Soft
   #endif
#endif