'Configuration settings for Compact Flash drive connected to ports

Const Lo8port = Portc                                       'lo8 byte is connected to?
Const Lo8pin = Pinc                                         'lo8 byte is connected to?
Const Lo8dd = Ddrc                                          'lo8 byte is connected to?
Const Contport = Porta                                      'Control bits (A0-A2, CS0, CS1, WR, RD) are connected to ?
Const Contdd = Ddra                                         'Control bits (A0-A2, CS0, CS1, WR, RD) are connected to ?
Const A0bit = 0                                             'A0 is connected to ?
'NOTE A1 must be connected to A0+1, and A2 must be connected to A0+2
Const Cfwe = 4                                              'Write Enable is connected to bit ? of contport
Const Cfoe = 5                                              'Output Enable is connected to bit ? of contport
Const Cfce1 = 3                                             'Chip Enable 1 is connected to bit ? of contport
Const Media_is_rotating = 0                                 'Not on a CF card
Const Addressmask = &B00000111                              'address needs 3 bits
Const Adjustedaddressmask = 2 ^ A0bit * Addressmask         'adjusted address mask
Const Adjustedcontmask = Adjustedaddressmask + 2 ^ Cfwe + 2 ^ Cfoe + 2 ^ Cfce1       'adjusted control mask
Const Drive_type = 3                                        '1 = ATA, 2 = SD/MMC , 3 = Compact Flash

$lib "cfonport.lbx"
$external Getsec , Readsec , Initdrive , Putsec , Writesec
Declare Sub Initdrive()