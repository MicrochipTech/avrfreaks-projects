'Configuration for SD/MMC card connected to hardware SPI

Const Spiport = Portb                                       'SPI on AVR is on port?
Const Spidd = Ddrb                                          'SPI Data Direction on AVR is ?
Const Sdmmccs = 0                                           'The SD / MMC Chip select = Slave Select = Spiport.?
Const Media_is_rotating = 0                                 'Not on a SD or MMC card 
Const Drive_type = 2                                        '1 = ATA, 2 = SD/MMC
'SPI config for SD / MMC card
Config Spi = Hard , Data Order = Msb , Master = Yes , Polarity = High , Phase = 1 , Clockrate = 4 , Noss = 1
Sbi Spidd , Sdmmccs                                         'make SD / MMC Chip Select = output
Sbi Spiport , Sdmmccs                                       'make CS = high
Spiinit                                                     'init the SPI
sbi Spsr, spi2X                                             'Turbo 2X
$lib "cardonspi.lbx"
$external Getsec , Readsec , Initdrive , Putsec , Writesec , Identify_device
Declare Sub Initdrive()