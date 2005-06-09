'Wave recorder player By Phil
'Uses 3 ISRs
'Dac_newsample puts a new sample in the two PWM D to A converters
'Adc_newsample starts a A to D conversion
'Adc_put_sample puts the A to D conversion result into its corresponding FIFO (circular buffer)
'References
'Microsoft RIFF Resource Interchange File Format

'BASCOM-AVR IDE Version : 1.11.7.7
$crystal = 16000000                                         '16 MHz crystal
$hwstack = 60
$swstack = 60
$framesize = 60

'***************************************************************
'Includes
'***************************************************************
$Include Configcardonspi.bas                               'configuration for the SPI card connected to hardware SPI.
'OR
'$Include Configataonport.bas                               'configuration for the ATA drive connected to ports
'OR
'$Include Configcfonport.bas                               'configuration for the Compact Flash connected to ports
$Include Configclock.bas                                   'configuartion for the clock
$Include Configfat16_32.bas                                'configuration for FAT16 / FAT32

'***************************************************************
'Constants
'***************************************************************
Const Empty = 0
Const Notfull = 1
Const Notempty = Notfull
Const Full = &HFF
Const Rows = 20                                             'Number of rows on ansi terminal
Const Fifosize_w = 300                                      'size of the word fifo
Const Fifosize_w_lo = Fifosize_w And &HFF
Const Fifosize_w_hi =(fifosize_w And &HFF00) / &H100
Const Fifosize_b = Fifosize_w * 2                           'size of the byte fifo
Const Fifosize_b_lo = Fifosize_b And &HFF
Const Fifosize_b_hi =(fifosize_b And &HFF00) / &H100
Const No_warning = 0
Const Not_a_wav = 1
Const Not_a_riff = 2
Const Unsupported_fmt = 3
Const No_data = 4
'max multiply is the maximum value expected to use a multipling factor of the sample. Values above this means division of the sample.
Const Maxmul = 1 +((_xtal \ 8000) \ 256)                    '1+ ((_xtal / min sample freq) / max sample value @ 8 bit/sample)
Const Adc_mux_ch0 = &B11100000                              'Channel 0, internal 2.5V ref
Const Adc_mux_ch1 = &B11100001                              'Channel 1, internal 2.5V ref
Const Start_adc_4m = &B11000010                             'too noisy
Const Start_adc_2m = &B11000011                             'good to about 36k samples / sec
Const Start_adc_1m = &B11000100                             'good to about 28k samples / sec
Const Start_adc_500k = &B11000101                           'good to about 16k samples / sec only
Const Start_adc_250k = &B11000110                           'good for 8k only (min)
Record_play Alias Portb.5

'***************************************************************
'Global Dims
'***************************************************************
Dim Warningcode As Byte
Dim Filenumber As Word                                      'The current file number
Dim Filemask As String * 12
Dim Command As String * 1
Dim Chunksize As Long
Dim Formattag As Word
Dim Numchannels As Word
Dim Samplespersec As Long
Dim Avgbytespersec As Long
Dim Blockalign As Word
Dim Bitspersample As Word
Dim Fudgefactor As Word
Dim Ch1fifo_w(fifosize_w) As Word
Dim Ch2fifo_w(fifosize_w) As Word                           'word fifo
Dim Ch1fifo_b(fifosize_b) As Byte At Ch1fifo_w Overlay
Dim Ch2fifo_b(fifosize_b) As Byte At Ch2fifo_w Overlay      'byte fifo
Dim Putpointer As Word
Dim Takepointer As Word
Dim Fifostatus As Byte

'***************************************************************
'Declares
'***************************************************************
Declare Sub Dir2ansi(direntry As Word)
Declare Sub Ansilocate(byval Row As Byte , Byval Col As Byte)
Declare Sub Infile(byval Searchstring As String , Byval Maxfileoffset As Long)
Declare Sub X50clock()
Declare Sub Playwav(direntry As Word)
Declare Sub Recordwav()

'***************************************************************
'Interrupt Vectors
'***************************************************************
On Compare1a Dac_newsample , Nosave
On Compare1b Adc_newsample , Nosave
On Adc Adc_put_sample , Nosave

'***************************************************************
'Program Start
'***************************************************************
Chunksize = Chunksize * Chunksize                           'to include mul32 routine????
Set Ddre.4                                                  '50X sample rate clock out
Set Ddrb.6                                                  'Ch1 D/A out
Set Ddrb.7                                                  'Ch2 D/A out
Set Ddrb.5                                                  'record / play switch
Enable Interrupts
Gosub Ansicls                                               'clear the ANSI terminal
Print "Hello"
Gosub Initdrive                                             'init the drive
Print Drivemodel()
Waitms 2000                                                 'display model for a second
Gosub Initfat                                               'Init fat filesystem
If Fatstatus.fatis16 = 1 Then
   Print "FAT16"
Else
   Print "FAT32"
End If
Filemask = "STARTUP.WAV"
Filenumber = Find_file(filemask)                            'see if startup.wav exists in the root directory
If Filenumber <> &HFFFF Then                                'if found
   Call Playwav(filenumber)                                 'play it (just for fun)
End If
Filemask = "*.WAV"
Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)
Call Dir2ansi(filenumber)                                   'Lets display the DIR

Main:
Command = Inkey()
Command = Ucase(command)
Select Case Command
   Case "C"                                                 'Catalogue
      Call Dir2ansi(filenumber)
   Case "U"                                                 'Up
      Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)       'Find next valid file number
      Call Dir2ansi(filenumber)                             'Display dir
   Case "D"                                                 'Down
      Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 255)       'Find previous valid file number
      Call Dir2ansi(filenumber)                             'Display dir
   Case "O"                                                 'Open
      Call Open_file(filenumber)                            'open the file (or dir)
      Call Dir2ansi(filenumber)                             'Display dir
   Case "P"                                                 'Play
      Call Playwav(filenumber)
   Case "R"                                                 'Record
      Call Recordwav()
   Case "S"                                                 'Set
      Print "Set Date & Time"
      Print "date " ; Date$
      Print "time " ; Time$
      Input "DD/MM/YY " , Date$
      Input "HH:MM:SS " , Time$
      Print "new date " ; Date$
      Print "newtime " ; Time$
   Case "K"                                                 'Kill
      Print "Kill " ; Read_dir_name_ext(filenumber)
      Call Delete_file(filenumber)
      If Errorcode = Del_dir_wrng Then
         Print "cannot delete DIR"                          'Cannot delete a directory
         Goto Deleteafile_end
      End If
'we just deleted a  file, so find the next valid file
      Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)       'Find next valid file number
      Deleteafile_end:
      Call Dir2ansi(filenumber)
End Select
Goto Main

'***************************************************************
'DIR to ANSI
'***************************************************************
'Displays the directory on an ANSI terminal.
'The first row is for headings
'The second Row is reserved for the currently selected file (or dir).
Sub Dir2ansi(direntry As Word)
Local Currentrow As Byte , Direntrycopy As Word , Filematch As Byte
Direntrycopy = Direntry                                     'make a copy of direntry
Gosub Ansicls                                               'Clear terminal & locate 1st row
'make headings
Print Filemask;
Call Ansilocate(1 , 15)
Print "Size";
Call Ansilocate(1 , 21)
Print "wrt Date";
Call Ansilocate(1 , 30)
Print "wrt Time";
Call Ansilocate(1 , 39)
Print "crt Date";
Call Ansilocate(1 , 48)
Print "crt Time";
Call Ansilocate(1 , 57)
Print "lstac dt";
Currentrow = 2                                              'Second row and down is for dir
Do
   Call Ansilocate(currentrow , 1)                          'Locate current row
   Print Read_dir_name_ext(direntrycopy)                    'print the file name with dot ext
   Call Ansilocate(currentrow , 15)
   Print Read_dir_filesizestr(direntrycopy) ;
   Call Ansilocate(currentrow , 21)
   Print Read_dir_wrtdate(direntrycopy) ;
   Call Ansilocate(currentrow , 30)
   Print Read_dir_wrttime(direntrycopy) ;
   Call Ansilocate(currentrow , 39)
   Print Read_dir_crtdate(direntrycopy) ;
   Call Ansilocate(currentrow , 48)
   Print Read_dir_crttime(direntrycopy) ;
   Call Ansilocate(currentrow , 57)
   Print Read_dir_lstaccdate(direntrycopy)
   Incr Currentrow
   Direntrycopy = Next_file_match(filemask , Attrib_directory , Direntrycopy , 1)       'find the next file that matches the global var (filemask)
   If Direntrycopy = &HFFFF Then Exit Do                    'if we didn't fine one, then bug out
   If Direntrycopy = Direntry Then Exit Do                  'exit if direntry = what it was (number of entries < number of rows)
Loop Until Currentrow > Rows
End Sub


'***************************************************************
'Error Handle
'***************************************************************
Errorhandle:
'Does bugger all. All fatal errors end up here.
'In theory the program should never end up here
'If it does, it may be caused by hardware problems, FAT32 file format problems (run scandisk).
'NEVER by BUGS in my program NEVER NEVER.
Print ; "Error " ; Errorcode                                'Print some critical values
Print ; "F off " ; Diroffset ; " " ; Fileoffset
Print ; "Clus " ; Dircluster ; " " ; Filecluster
Print ; "first " ; Dirfirstcluster ; " " ; Filefirstcluster
Print ; "C Idx " ; Dirclusterindex ; " " ; Fileclusterindex
Print ; "eof " ; Dirstatus ; " " ; Filestatus
#if Media_is_rotating = 1
   Gosub Gotosleep                                          'Just in case drive is alive, put it to sleep
#endif
End                                                         'end program
Return                                                      'never return

'***************************************************************
'ANSI Clear Screen
'***************************************************************
Ansicls:
Printbin 27 ; 91;                                           'escape [
Print "2J";                                                 'erase screen
Return

'***************************************************************
'ANSI Locate
'***************************************************************
Sub Ansilocate(byval Row As Byte , Byval Col As Byte)
Printbin 27 ; 91;                                           'escape [
Print Row ; ";" ; Col ; "H";                                'force cursor position
End Sub

'***************************************************************
'ANSI Clear current line
'***************************************************************
Ansiclrline:
Printbin 27 ; 91;                                           'escape [
Print "2K";                                                 'erase the current line
Return

'***************************************************************
'Record a Wave file
'***************************************************************
Sub Recordwav()
Local Wav_filename As String * 20 , Wave_template_data As Byte , Ext_pos As Byte , Mono_stereo As String * 1
Formattag = 1                                               'Always 1 = PCM
Numchannels = 2                                             'by default 2 Channels
Input "Mono or Stereo " , Mono_stereo
Mono_stereo = Ucase(mono_stereo)
If Mono_stereo = "M" Then Numchannels = 1                   'If mono, then 1 Channel
Input "samples per sec " , Samplespersec
If Samplespersec < 8000 Then
   Print "default to 8000"
   Samplespersec = 8000                                     'min samples per second for telephone quality
End If
If Samplespersec > 40000 Then
   Print "default to 36000"
   Samplespersec = 36000                                    'approx max samples per second for adc & file system
End If
Avgbytespersec = Samplespersec * Numchannels
Blockalign = Numchannels
Bitspersample = 8
Recordwav_filename:
Input "filename " , Wav_filename
Ext_pos = Instr(wav_filename , "." )                        'find the position of the extension
If Ext_pos = 0 Then Ext_pos = Len(wav_filename) + 1         'if no extension found
Mid(wav_filename , Ext_pos) = ".WAV"                        'add the .WAV extension in either case
Filenumber = Find_file(wav_filename)
If Filenumber <> &HFFFF Then
   Print Wav_filename ; " exists"
   Call Delete_file(filenumber)                             'delete the file first
End If
Filenumber = Create_file(wav_filename , Attrib_archive)     'create a file
If Errorcode = File_str_empty_wrng Then
   Print "enter ";
   Goto Recordwav_filename                                  'File name must not be an empty string
End If
Call Open_file(filenumber)                                  'open the created file
Print Wav_filename ; " created"
Restore Wave_template
Do
   Read Wave_template_data
   Call Write_sequential(wave_template_data)                'write the basic template
Loop Until Fileoffset => 20                                 '20 bytes
'Write 16 bytes of the fmt chunk
Call Write_16(formattag)
Call Write_16(numchannels)
Call Write_32(samplespersec)
Call Write_32(avgbytespersec)
Call Write_16(blockalign)
Call Write_16(bitspersample)
Do
   Read Wave_template_data
   Call Write_sequential(wave_template_data)                'write the start of the data chunk
Loop Until Fileoffset => 44

'Counter 1 is used to sync samples
'Waveform generation mode 4, top = compare 1 A, interrupt on compare 1B
Tccr1a = &B00001000
Tccr1b = &B00001001
Samplespersec = _xtal / Samplespersec
Decr Samplespersec
Compare1a = Samplespersec                                   'Set TOP Counter1
Compare1b = Samplespersec                                   'set interupt count
Call X50clock()                                             'generate a 50 X sampe rate clock for filter
Reset Record_play                                           'record switch
Set Adcsr.aden                                              'turn on adc
Admux = Adc_mux_ch0
Adcsr = Start_adc_2m                                        'start conversion, and dump result
Waitms 1000                                                 'wait for coupling capacitors to charge
Adcsr = Start_adc_2m                                        'start conversion, and dump result
Waitms 1000                                                 'wait for coupling capacitors to charge
Print "recording"
Putpointer = 0                                              'Initialise fifo control
Takepointer = 0
Fifostatus = Empty
Counter1 = 0
Set Timsk.ocie1b                                            'Set OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
'Interrupt is a go, lets record
Do
   While Fifostatus = Notempty                              'while fifo not empty
      Call Write_sequential(ch1fifo_b(takepointer + 1))     'write ch1. Bascom counts arrays base 1
      If Numchannels = 2 Then
         Call Write_sequential(ch2fifo_b(takepointer + 1))  'write ch2
      End If
      'written in asm to execute faster
      lds r24, {takepointer}
      lds r25, {takepointer+1}
      adiw r24, 1                                           'incrument take pointer
      ldi r16, Fifosize_b_lo
      ldi r17, Fifosize_b_hi
      cp r16, r24
      cpc r17, r25                                          'compare with fifo size
      brne recnowrap
      clr r24
      clr r25                                               'back to zero, if we have wrapped
      Recnowrap:
      lds r16, {putpointer}
      lds r17, {putpointer+1}
      cp r16, r24
      cpc r17, r25                                          'compare put & take pointers
      ldi r16, empty                                        'by default
      breq recupdatestatus
      ldi r16, notempty
      Recupdatestatus:
      sts {fifostatus}, r16                                 'save status
      sts {takepointer}, r24
      sts {takepointer+1}, r25                              'save pointer
   Wend
Loop Until Inkey() = 27                                     'record until escape is pressed
Reset Timsk.ocie1b                                          'clear OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
'Interrupt is a no go
Reset Adcsr.aden                                            'turn adc off
Tccr1a = 0                                                  'Timer Counter Control 1A = off
Tccr1b = 0                                                  'Timer Counter Control 1B = off
Tccr3a = 0                                                  'Timer Counter Control 3A = off
Tccr3b = 0                                                  'Timer Counter Control 3B = off
Gosub Close_file                                            'close the file.
Call Open_file(filenumber)                                  'open the file again
Chunksize = Filesize - 46                                   'Calculate the size of the data chunk
Fileoffset = 40
Gosub Random_access                                         'random access to file offset 40
Call Write_32(chunksize)                                    'write the chunk size of the data chunk
Set Fatstatus.randommode                                    'force a flush back to the original position
Gosub Close_file
Print "finished"
End Sub

'***************************************************************
'Generate a 50 X sample rate clock for switched cap filter
'***************************************************************
Sub X50clock()
Local Ctr As Word
Ctr = Compare1a                                             'counter1A contains the magic number to which correspond to the samples / sec
Incr Ctr
Ctr = Ctr / 100                                             '50X and another 2X because we are using toggle on match
Decr Ctr
'Counter 3 is used to generate a 50 X sample rate square wave (1:1 mark:space ratio) clock for switched cap filter
'Waveform generation mode 4, toggle on match, TOP = compare 3 A
Tccr3a = &B00010000
Tccr3b = &B00001001
Compare3a = Ctr                                             'Set TOP Counter3
'A 50 X sample rate clock should pop out of OC3B
End Sub

'***************************************************************
'In File
'***************************************************************
'File must be opened first.
'Searches for a string in a file starting from the current file offset.
'Returns the file offset at the end of the string.
'If found, the warning code is set to zero to indicate success
'So if the file offset of the start of string is needed, then subtract the length of the search string.
'If the search string is not found before max file offset, then warning code is left unchanged to indicate failure
Sub Infile(byval Searchstring As String , Byval Maxfileoffset As Long)
Local Offset As Byte , Searchchr As String * 1 , Foundbyte As Byte
Offset = 1                                                  'Lets start at offset 1 of the search string
Do
   Searchchr = Mid(searchstring , Offset , 1)               'Assign search character
   Foundbyte = Read_sequential()                            'Assign Found byte
   If Searchchr = Foundbyte Then
      If Offset = Len(searchstring) Then                    'Check to see if we are done
         Warningcode = No_warning                           'Then clear warning
         Exit Do                                            'And bug out
      End If
      Incr Offset                                           'Next search character
   Else
      If Fileoffset > Maxfileoffset Then Exit Do            'if time to give up then bug out
      Offset = 1                                            'Start searching at offset 1 again
   End If
Loop
End Sub

'***************************************************************
'Play a Wave file
'***************************************************************
'parses the wav file, sets up PWM on counter 1 generates 50X sample rate clock on counter 0, and plays wav file
Sub Playwav(direntry As Word)
If Read_dir_ext(direntry) <> "WAV" Then                     'make sure it is a WAV file
   Print "not a WAV"
   Exit Sub
End If
Call Open_file(direntry)                                    'open the file
Print "playing " ; Read_dir_name_ext(direntry)
Gosub Parsewav                                              'check out the wav file and pull out format info
If Warningcode > 0 Then                                     'If parsewav cannot find format info,
   Print "WAV error " ; Warningcode
   Goto Playwavend                                          'then bug out
End If
If Formattag <> 1 Then                                      'if Formattag is not 1 (uncompressed raw pcm)
   Print "Unsupported WAV"
   Goto Playwavend                                          'then bug out
End If
Filesize = Chunksize + 46
'modify the file size to exclude extra stuff, like artist, company info, etc that is tacked on the end sometimes
'Counter 1 is used as a PWM Digital to Analogue converter
'waveform generation mode 15, TOP = OCR1A
Tccr1a = &B00101011
Tccr1b = &B00011001
Samplespersec = _xtal / Samplespersec
Decr Samplespersec
Compare1a = Samplespersec                                   'Set TOP Counter1
'Fudge factor is a multiply if we are doing 8 bit or divide if 16 bit.
'In 8 bit WAV uses more of the available range of the compare register.
'In 16 bit WAV makes sure the compare register is less than TOP by chopping off the LSBs.
Fudgefactor = Samplespersec / &H00FF                        'by default
If Bitspersample = 16 Then Fudgefactor = &HFFFF / Samplespersec
Set Record_play                                             'play
Call X50clock()
Putpointer = 0                                              'Initialise fifo control
Takepointer = 0
Fifostatus = Empty
Counter1 = 0
Gosub Fillfifo                                              'Give the fifo a head start
Set Timsk.ocie1a                                            'Set OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
'Interrupt is a go, Lets play
Do
   Gosub Fillfifo                                           'keep the fifo full
Loop Until Inkey() = 27                                     'play until escape is pressed
Playwaveeof:
'Finished Playing Wav File, but there are still a few bytes left in the fifo
While Fifostatus <> Empty
   'hang around
Wend
Playwavend:
Reset Timsk.ocie1a                                          'Clear OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
'Interrupt is a NO go
Tccr1a = 0                                                  'Timer Counter Control 1A = off
Tccr1b = 0                                                  'Timer Counter Control 1B = off
Tccr3a = 0                                                  'Timer Counter Control 3A = off
Tccr3b = 0                                                  'Timer Counter Control 3B = off
Gosub Close_file
Print "finished playing " ; Read_dir_name_ext(direntry)
End Sub

'***************************************************************
'Parse a Wave file
'***************************************************************
Parsewav:
Warningcode = Not_a_riff
Call Infile( "RIFF" , 100)                                  'find the text "RIFF"
If Warningcode <> No_warning Then Return
Warningcode = Not_a_wav
Call Infile( "WAVE" , 100)                                  'find the text "WAVE"
If Warningcode <> No_warning Then Return
Warningcode = Unsupported_fmt
Call Infile( "fmt " , 100)                                  'find the text "fmt "
If Warningcode <> No_warning Then Return
Chunksize = Read_32()                                       'read the format chunk size (not used)
Formattag = Read_16()                                       'read the format chunk tag
Numchannels = Read_16()                                     'read the format chunk num of ch
Samplespersec = Read_32()                                   'read the format chunk samples per sec
Avgbytespersec = Read_32()                                  'read the format chunk average bytes per sec
Blockalign = Read_16()                                      ''read the format chunk block align (must = num of ch * bits per sample)
Bitspersample = Read_16()                                   'read the format chunk bits per sample
Warningcode = No_data
Call Infile( "data" , 100)                                  'find the text "data"
If Warningcode <> No_warning Then Return
Chunksize = Read_32()                                       'read the size of the data chunk
'Print "chunksize" ; Chunksize
'Print "filesize" ; Filesize
'Print "Formattag" ; Formattag
'Print "Numchannels" ; Numchannels
'Print "Samplespersec" ; Samplespersec
'Print "Avgbytespersec" ; Avgbytespersec
'Print "Blockalign" ; Blockalign
'Print "Bitspersample" ; Bitspersample
Return

'***************************************************************
'Fill the CH1 & CH2 FIFO with fudged words until full
'***************************************************************
Fillfifo:
lds r16, {fifostatus}
cpi r16, full
brne fillfifofill                                           'While fifo not full, fill it
ret                                                         'RETURN is here
Fillfifofill:
lds r16, {Filestatus}
sbrc r16, endoffileflag                                     'While not at end of file
rjmp Fillfifoeof                                            'return to playwav end of file
'Above code = While fifo not full AND While filestatus not at end of file
rcall Getsample                                             'comes back with fudged ch word in r16-r17
push r16
push r17                                                    'save just in case we have 2 channels
lds r18, {Numchannels}
cpi r18, 2                                                  'if we have 2 channels
brne fillfifomono                                           'skip getting ch2 byte
rcall Getsample                                             'comes back with fudged ch word in r16-r17
Fillfifomono:
movw r20, r16                                               'copy Ch2 word, in either case
pop r17
pop r16                                                     'pop Ch1 word in either case
lds r24, {putpointer}
lds r25, {putpointer+1}                                     'put pointer to r24-r25
lsl r24
rol r25                                                     'multiply by bytes per word (2) since the fifo contains words
Loadadr Ch1fifo_w(1) , X
add r26, r24
adc r27, r25                                                'add pointer to fifo address
st X+, r16                                                  'Store Ch1 lo
st X, r17                                                   'Store Ch1 hi
Loadadr Ch2fifo_w(1) , X
add r26, r24
adc r27, r25                                                'add pointer to fifo address
st X+, r20                                                  'Store Ch2 lo
st X, r21                                                   'store Ch2 hi
lsr r25
ror r24                                                     'divide pointer by bytes per word (2)
adiw r24, 1                                                 'incument put pointer
ldi r18, Fifosize_w_lo
ldi r19, Fifosize_w_hi
cp r18, r24
cpc r19, r25                                                'compare with fifo size
brne fillfifonowrap
clr r24
clr r25                                                     'back to 0 if we have wrapped
Fillfifonowrap:
sts {putpointer}, r24
sts {putpointer+1}, r25                                     'Store pointer
lds r18,{takepointer}
lds r19, {takepointer+1}                                    'Take pointer to r16-r17
cp r24, r18
cpc r25, r19                                                'compare take pointer with put pointer
ldi r16, notfull
brne fillfifoend
ldi r16, full
Fillfifoend:
sts {fifostatus}, r16                                       'update fifo status
rjmp fillfifo                                               'again
Fillfifoeof:
pop r0
pop r1                                                      'pop off return address
rjmp Playwaveeof                                            'and exit Playwave do loop

'***************************************************************
'Get a 8 bit or 16 bit sample & fudge it
'***************************************************************
Getsample:
ldi Xl, 0
ldi Xh, 0                                                   'address of r0 to X
st -Y, Xh                                                   'save address to soft stack (hi first)
st -Y, Xl                                                   'and grow soft stack down by 1 word address
lds r18, {Bitspersample}
cpi r18, 16
breq Getsample16bit:
!call Read_sequential                                       'get a 8 bit sample in r0
clr r1                                                      'clear hi 8
rjmp Getsampledone
Getsample16bit:
!call read_16                                               'get a 16 bit sample in r0-r1
Getsampledone:
adiw Yl, 2                                                  'release soft stack
movw r16, r0
'r16-r17 contain the 16 bit sample or 8 bit sample with hi byte = 0
lds r20, {Fudgefactor}
cpi r20, maxmul
brlt getsamplemulsample
ldi r22, &HFF                                               'bias for +ve values lo OR
ldi r23, &H7F                                               'xor mask for -ve values (complement) hi
sbrs r17, 7                                                 'if sign bit clear
rjmp Getsamplepos                                           'must be a +ve number
'16 bit samples are in twos complement form
eor r16, r22                                                'complement lo
eor r17, r23                                                'complement hi
subi r16, 1*-1                                              'add 1
sbci r17, 1*-1                                              'turn into unsigined 16 bit
Getsampledivneg:                                            'Dodgy fast divide
lsr r17                                                     'shift right (divide by 2)
ror r16                                                     'rotate right thu carry (divide by 2)
lsr r20                                                     'shift fudge factor right
brne Getsampledivneg                                        'till fudge factor = 0
ret                                                         '16 bit -ve return
Getsamplepos:
!sub r22, r16                                               'subtract from bias lo
sbc r23, r17                                                'subtract from bias hi
movw r16, r22
Getsampledivpos:                                            'Dodgy fast divide
lsr r17                                                     'shift right (divide by 2)
ror r16                                                     'rotate right thu carry (divide by 2)
lsr r20                                                     'shift fudge factor right
brne Getsampledivpos                                        'till fudge factor = 0
ret                                                         '16 bit +ve return
Getsamplemulsample:
'Multiply 8 bit sample to get more volume
mul r16, r20                                                'multiply sample by fudge factor
movw r16, r0
ret                                                         '8 bit return

'***************************************************************
'Analoge to Digital Converter Put Sample Interrupt Service Routine
'***************************************************************
'If stero and if the conversion result belongs to CH1, then it is stored, and CH2 conversion is started.
'If mono, then CH1 is stored and put pointer & fifo status are updated.
'If the conversion result belongs to CH2, then it is stored, and put pointer & fifo status are updated.
'This is called by interrupt, so all registers must stay un corrupted.
'With NoSave, must save used registers
Adc_put_sample:
push r16                                                    'save used registers
in r16, sreg
push r16
push r17
push r24
push r25
push r26
push r27
lds r24, {putpointer}
lds r25, {putpointer+1}                                     'get put pointer
lds r17, {Numchannels}                                      'number of channels in r17
sbic admux, 0                                               'if the current result is CH2
rjmp adc_put_sample_2                                       'then put CH2 result
'otherwise the current result belongs to CH1
in r16, adch                                                'get ADC result in r16
sbrs r17, 1                                                 'if mono
rjmp Adc_put_no_ch2                                         'then there is no need to start CH2 conversion
sbi admux, 0                                                'switch ADC to Channel 2
sbi Adcsr, adsc                                             'start conversion, channel 2
sbi adcsr, adie                                             'enable adc interrupt
Adc_put_no_ch2:
Loadadr Ch1fifo_b(1) , X
add r26, r24
adc r27, r25                                                'add putpointer to address
st X, r16                                                   'store result
sbrs r17, 1                                                 'if mono
rjmp Adc_put_sample_ptr                                     'update pointers
pop r27                                                     'restore used registers
pop r26
pop r25
pop r24
pop r17
pop r16
!Out Sreg, r16
pop r16
reti                                                        'CH1 return from interrupt.
Adc_put_sample_2:
cbi adcsr, adie                                             'disable adc interrupt. It will be enabled again on next sample
Loadadr Ch2fifo_b(1) , X
add r26, r24
adc r27, r25                                                'add putpointer to address
in r16, adch                                                'get ADC result
st X, r16                                                   'store result
Adc_put_sample_ptr:
adiw r24, 1                                                 'incrument put pointer
ldi r16, fifosize_b_lo
ldi r17, fifosize_b_hi
cp r24, r16
cpc r25, r17                                                'compare with fifosize
brne Adc_put_nowrap
clr r24                                                     'if we have reached fifo size, set putpointer back to 0
clr r25
Adc_put_nowrap:
lds r16, {takepointer}
lds r17, {takepointer+1}                                    'take pointer to r16-r17
cp r16 , r24
cpc r17, r25                                                'compare take pointer and put pointer
ldi r16, full
breq Adc_put_updateptr                                      'if FIFO is full, don't update put pointer.
ldi r16, notfull
sts {putpointer}, r24
sts {putpointer+1}, r25                                     'Save new putpointer
Adc_put_updateptr:
sts {fifostatus}, r16                                       'Update fifo status &HFF = fifo full, 1= not full
pop r27                                                     'restore used registers
pop r26
pop r25
pop r24
pop r17
pop r16
!Out Sreg, r16
pop r16
reti                                                        'CH2 return from interrupt, or mono return from interrupt.

'***************************************************************
'Analoge to Digital Converter New Sample Interrupt Service Routine
'***************************************************************
'Starts CH1 A/D conversion , & enables adc iterrupt
'This is called by interrupt, so all registers must stay un corrupted.
'With NoSave, must save used registers.
'NOTE SREG is not used.
Adc_newsample:
push r16
ldi r16, Adc_mux_ch0
!out Admux, r16
sbi Adcsr, adsc                                             'start conversion, channel 1, left adjust result
sbi adcsr, adie                                             'enable adc interrupt
pop r16
reti

'***************************************************************
'Digital to Analoge Converter New Sample Interrupt Service Routine
'***************************************************************
'Sticks a new sample into the Pulse Width Modulator compare registers, from fifos
'Updates take pointer and fifo status.
'This is called by interrupt, so all registers must stay un corrupted.
'With NoSave, must save used registers
Dac_newsample:
push r16
in r16, sreg
push r16
push r17
push r24
push r25
push r26
push r27                                                    'Save used registers
lds r24, {takepointer}
lds r25, {takepointer+1}                                    'get take pointer
lsl r24
rol r25                                                     'multiply by bytes per word (2) since the fifo contains words
Loadadr Ch1fifo_w(1) , X
add r26, r24
adc r27, r25                                                'add takepointer to address
ld r16, X+                                                  'get the pwm lo byte
ld r17, X                                                   'get the pwm hi
!out ocr1bh, r17                                            'High register first for this 16 bit register
!out ocr1bl, r16                                            'Lo register next. The CH1fifo(takepointer) byte
Loadadr Ch2fifo_w(1) , X
add r26, r24
adc r27, r25                                                'add takepointer to address
ld r16, X+                                                  'get the pwm lo byte
ld r17, X                                                   'get the pwm hi
!out Ocr1ch, r17                                            'High register first for this 16 bit register
!out Ocr1cl, r16                                            'Lo register next. The CH1fifo(takepointer) byte
lsr r25
ror r24                                                     'divide pointer by bytes per word (2)
adiw r24, 1                                                 'incrument takepointer
ldi r16, Fifosize_w_lo
ldi r17, Fifosize_w_hi
cp r24, r16
cpc r25, r17                                                'compare with fifosize
brne dacnewsamplenowrap
clr r24                                                     'if we have reached fifo size, set takepointer back to 0
clr r25
Dacnewsamplenowrap:
lds r16, {putpointer}
lds r17, {putpointer+1}                                     'put pointer to r16-r17
cp r16 , r24
cpc r17, r25                                                'compare take pointer and put pointer
ldi r16, empty
breq dacnewsamplenoupdateptr                                'on next sample use the same PWM word if take pointer and put pointer are equal
ldi r16, notempty
sts {takepointer}, r24
sts {takepointer+1}, r25                                    'Save new takepointer if no under run
Dacnewsamplenoupdateptr:
sts {fifostatus}, r16                                       'Update fifo status 0 = fifo empty, 1= not empty
pop r27                                                     'Restore used registers
pop r26
pop r25
pop r24
pop r17
pop r16
!Out Sreg, r16
pop r16
reti

Wave_template:
'Chunk ID = "RIFF", Size = 4, Format = "WAVE"
Data &H52 , &H49 , &H46 , &H46 , &H24 , &H08 , &H00 , &H00 , &H57 , &H41 , &H56 , &H45
'Chunk ID = "fmt ",  Size = 16
Data &H66 , &H6D , &H74 , &H20 , &H10 , &H00 , &H00 , &H00  '20 bytes to here
'Then 16 bytes of fmt chunk
'chunkid = "DATA", chunksize = 0
Data &H64 , &H61 , &H74 , &H61 , &H0 , &H0 , &H0 , &H0      'Another 8 bytes, total 44 bytes