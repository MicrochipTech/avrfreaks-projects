'Configuration settings for ATA drive connected to ports

Const Media_is_rotating = 1                                 'Is the media rotating?
Const Drive_is_master = 0                                   'Drive is? 1 = strapped for Master, 0 = strapped for slave
Const Lo8port = Portc                                       'lo8 byte is connected to?
Const Lo8pin = Pinc                                         'lo8 byte is connected to?
Const Lo8dd = Ddrc                                          'lo8 byte is connected to?
Const Hi8port = Porta                                       'Hi8 byte is connected to?
Const Hi8pin = Pina                                         'Hi8 byte is connected to?
Const Hi8dd = Ddra                                          'Hi8 byte is connected to?
Const Contport = Portd                                      'Control bits (A0-A2, CS0, CS1, WR, RD) are connected to ?
Const Contdd = Ddrd                                         'Control bits (A0-A2, CS0, CS1, WR, RD) are connected to ?
Const A0bit = 0                                             'A0 is connected to ?
'NOTE A1 must be connected to A0+1, and A2 must be connected to A0+2
Const Atawr = 5                                             'Write is connected to bit ? of contport
Const Atard = 6                                             'Read is connected to bit ? of contport
Const Atacs0 = 3                                            'Chip select 0 is connected to bit ? of contport
Const Atacs1 = 4                                            'Chip select 1 is connected to bit ? of contport
'NOTE if Atacs1 (ATA Chip Select 1) not used make it >= 8
#if Media_is_rotating = 1
Const Maxtime = 20                                          'Max Time for drive to complete task (in seconds)?
Const Sleeptime = 30                                        'Put drive to sleep after Sleeptime seconds of no reads or writes?
#endif
Const Addressmask = &B00000111                              'address needs 3 bits
Const Adjustedaddressmask = 2 ^ A0bit * Addressmask         'adjusted address mask
#if Atacs1 < 8
Const Adjustedcontmask = Adjustedaddressmask + 2 ^ Atawr + 2 ^ Atard + 2 ^ Atacs0 + 2 ^ Atacs1       'adjusted control mask
#else
Const Adjustedcontmask = Adjustedaddressmask + 2 ^ Atawr + 2 ^ Atard + 2 ^ Atacs0       'adjusted control mask
#endif
Const Drive_type = 1                                        '1 = ATA or CF in true IDE mode, 2 = SD/MMC, 3 = CF

#if Drive_is_master
'ATA device / head register bits
.equ Devlba = &B01000000                                    'LBA mode and device 0 (master). Must be LBA mode.
#else
'ATA device / head register bits
.equ Devlba = &B01010000                                    'LBA mode and device 1 (slave). Must be LBA mode.
#endif
'Variables Used By Ataonport.lib
#if Media_is_rotating = 1
Dim Ticktock As Byte                                        'System clock, goes up by 1 every second, wraps at 255 (4.25min)
Dim Timetosleep As Byte                                     'Contains the system time that the drive is due to sleep
#endif
$lib "ataonport.lbx"
$external Getsec , Readsec , Gotosleep , Initdrive , Putsec , Writesec
Declare Sub Initdrive()