'Fat Test By Phil
'BASCOM-AVR IDE Version : 1.11.7.7
$hwstack = 70
$swstack = 70
$framesize = 70
'Mass storage device driver
'$Include Configcardonspi.bas                               'configuration for the SPI card connected to hardware SPI.
'OR
$Include Configataonport.bas                               'configuration for the ATA drive connected to ports
'OR
'$Include Configcfonport.bas                               'configuration for the Compact Flash connected to ports
'Clock configuartion and driver
$Include Configclock.bas                                   'configuartion for the clock

$Include Configfat16_32.bas                                'configuration for FAT16 / FAT32

Const Rows = 20                                             'Number of rows on ansi terminal

Declare Sub Dir2ansi(direntry As Word)
Declare Sub Debugdump2ansi(direntry As Word)
Declare Sub Ansilocate(byval Row As Byte , Byval Col As Byte)
Declare Function Alpha_num_filt(unfilteredstr As String)as String
Declare Sub Makeadir()
Declare Sub Testwrite()
Declare Sub Pathtst()

Dim Filenumber As Word                                      'The current file number
Dim Filebyte As Byte                                        'The byte to or from the file
'Dim Fileword As Word
'Dim Filelong As Long
Dim Templong As Long
Dim Temp As Byte
Dim Tempword As Word
Dim Filemask As String * 12
Dim Command As String * 1
Dim Ltimestamp As Long
Dim Lsecondselapsed As Long
Dim ___rseed As Word
Dim Attrib As Byte

'***************************************************************
'Program Start
'***************************************************************
#if Clock_is_ds1307 = 1
   #if Media_is_rotating = 1
      Enable Interrupts                                     'hardware clock & rotating media
   #endif
#else
   #if Media_is_rotating = 1
      Enable Interrupts                                     'software clock & rotating media
   #else
      Enable Interrupts                                     'software clock & non rotating media
   #endif
#endif
'Interrupts NOT needed for non rotating media AND with a ds1307 clock.

Gosub Ansicls                                               'clear the ANSI terminal
Print "Hello"
Call Initdrive()                                            'init the drive
Print Drivemodel()
Waitms 2000                                                 'display model for a second
#if Clock_is_ds1307 = 1
   Print Date$
   Print Time$
#else
   'Input "Day Month Year " , _day , _month , _year
   'Input "Hour Min Sec " , _hour , _min , _sec
   _day = 2
   _month = 6
   _year = 5
   _hour = 0
   _min = 0
   _sec = 0
#endif
Call Initfat()                                              'Init fat filesystem
If Fatstatus.fatis16 = 1 Then
   Print "FAT16"
Else
   Print "FAT32"
End If
Print "Plbabegin " ; Plbabegin
Print "Bpb_secperclus " ; Bpb_secperclus
Print "Bpb_resvdseccnt " ; Bpb_resvdseccnt
Print "Bpb_numfats " ; Bpb_numfats
Print "Bpb_totsec " ; Bpb_totsec
Print "Bpb_fatz " ; Bpb_fatz
Print "Bpb_rootclus " ; Bpb_rootclus
Print "Bpb_fsinfo " ; Bpb_fsinfo
Print "Dirlbabegin " ; Dirlbabegin
Print "Cntofclusters " ; Cntofclusters
Print "Bytesperclus " ; Bytesperclus
Print "fatlbabegin " ; Fatlbabegin
Print "Root_dir_sectors " ; Root_dir_sectors
Print "Filenumofit " ; Filenumofit
Print "Nxt_free " ; Nxt_free
Print "Free_count " ; Free_count
'Templong = Free_count * Bytesperclus                        'Wraps at 4.3Gbytes (max 32bit number) & max +ve long in bascom = 214,7483,647 (214M)
'Print "free bytes on disk " ; Templong
'Templong = Cntofclusters * Bytesperclus                     'Wraps at 4.3Gbytes (max 32bit number) & max +ve long in bascom = 214,7483,647 (214M)
'Print "total bytes on disk " ; Templong
Filemask = "*.*"
'Call Next_file(filenumber , 1)                               'find the first valid file
Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)
Call Dir2ansi(filenumber)                                   'Lets display the DIR

Do
   Command = Inkey()
   Command = Ucase(command)
   Select Case Command
   Case "E"
        Call Pathtst()
   Case "O"
      Gosub Openafile
      Call Dir2ansi(filenumber)
   Case "D"
      Print "Down"
      Gosub Down
   Case "U"
      Print "Up"
      Gosub Up
   Case "R"
      Print "Read"
      Call Debugdump2ansi(filenumber)
      Call Dir2ansi(filenumber)
   Case "K"
      Print "Kill"
      Gosub Deleteafile
      Call Dir2ansi(filenumber)
   Case "M"
      Print "Make"
      Gosub Makeadir
      Call Dir2ansi(filenumber)
   Case "S"
      Print "Set Date & Time"
      Print "date " ; Date$
      Print "time " ; Time$
      Input "DD/MM/YY " , Date$
      Input "HH:MM:SS " , Time$
      Print "new date " ; Date$
      Print "newtime " ; Time$
      Gosub Anykey
      Call Dir2ansi(filenumber)
   Case "T"
      Print "Test"
      Call Testwrite()                                      'the BIG test
      Call Dir2ansi(filenumber)
   Case "B"
      Print "Print Backward"
      Gosub Printafilebackwards:
      Call Dir2ansi(filenumber)
   Case "Q"
      Print "Move a file"
      Gosub Moveafile
      Call Dir2ansi(filenumber)
   Case "P"
      Print "Print"
      Gosub Printafile
      Call Dir2ansi(filenumber)
   Case "F"
      Print "file mask " ; Filemask ; " new mask?"
      Input "file mask " , Filemask
      Call Dir2ansi(filenumber)
   Case ""
      'do nothing
   Case Else
      Print "No command " ; Command;
      Wait 1
      Gosub Ansiclrline
   End Select
Loop

Anykey:
Print "Anykey to continue"
Command = Waitkey()
Return

Sub Makeadir()
Local Dirname As String * 15
Makedir_entername:
Input "dirname " , Dirname
Filenumber = Find_file(dirname)                             'see if it already exists
If Filenumber <> &HFFFF Then                                'if returns &HFFFF, then it does not exist
   Print Dirname ; " exists"
   Goto Makedir_entername
End If
Filenumber = Create_file(dirname , Attrib_directory)        'create a directory
Gosub Anykey
End Sub

Deleteafile:
Print "deleting " ; Read_dir_name_ext(filenumber)
Call Delete_file(filenumber)
If Errorcode = Del_dir_wrng Then
   Print "cannot delete DIR"                                'Cannot delete a directory
   Goto Deleteafile_end
End If
'we just deleted a  file, so find the next valid file
'Call Next_file(filenumber , 1)                               'Find next valid file number
Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)
Deleteafile_end:
Gosub Anykey
Return

Openafile:
Print Read_dir_name_ext(filenumber) ; " Opened " ; Read_dir_filesize(filenumber) ; " bytes"
Call Open_file(filenumber)                                  'open the file (or dir)
Gosub Anykey
Return

Up:
'Call Next_file(filenumber , 1)                               'Find next valid file number
Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 1)       'find the next file that matches the global var "filemask"
Call Dir2ansi(filenumber)                                   'Display dir on terminal
Return

Down:
'Call Next_file(filenumber , 255)                             'Find previous valid file number
Filenumber = Next_file_match(filemask , Attrib_directory , Filenumber , 255)       'find the previous file that matches the global var "filemask"
Call Dir2ansi(filenumber)                                   'Display dir on terminal
Return

Printafilebackwards:
Attrib = Read_dir_attr(filenumber)
If Attrib.attrib_directorybit = 1 Then
   Print "Cannot print <DIR>"
   Goto Printafilebackwards_end
End If
Print "Printing backwards " ; Read_dir_name(filenumber)
Call Open_file(filenumber)                                  'open the file
For Fileoffset = Filesize To 0 Step -1
   If Inkey() = 27 Then Exit For
   Filebyte = Read_random()                                 'Read a byte randomly at file offset
   Print Chr(filebyte);                                     'print the file byte
Next Fileoffset
Print
Call Close_file()
Print
Printafilebackwards_end:
Gosub Anykey
Return

Printafile:
Attrib = Read_dir_attr(filenumber)
If Attrib.attrib_directorybit = 1 Then
   Print "Cannot print <DIR>"
   Goto Printafile_end
End If
Print "Printing " ; Read_dir_name_ext(filenumber)
Call Open_file(filenumber)                                  'open the file
While Filestatus.endoffileflag = 0
   If Inkey() = 27 Then Exit While
   Filebyte = Read_sequential()                             'Read a byte sequentialy
   Print Chr(filebyte);
Wend
Call Close_file()
Print
Printafile_end:
Gosub Anykey
Return

Moveafile:
Attrib = Read_dir_attr(filenumber)
If Attrib.attrib_directorybit = 1 Then
   Print "Cannot move <DIR>"
   Goto Moveafile_end
End If
Print "moving " ; Read_dir_name(filenumber)
Call Open_file(filenumber)                                  'open the file
#if Clock_is_ds1307 = 1
   Gosub Getdatetime                                        'update _sec, _min _hour variables if using ds1307
#endif
Ltimestamp = Secofday()                                     'Set time stamp
While Filestatus.endoffileflag = 0
   Filebyte = Read_sequential()
   If Inkey() = 27 Then Exit While
Wend
Print
Print "Moved " ; Fileoffset ; " bytes"
#if Clock_is_ds1307 = 1
   Gosub Getdatetime                                        'update _sec, _min _hour variables if using ds1307
#endif
Lsecondselapsed = Secelapsed(ltimestamp)                    'calc the elapsed seconds
Print "In " ; Lsecondselapsed ; " seconds"
Lsecondselapsed = Fileoffset / Lsecondselapsed              'let's not fuss too much about divide by zero errors
Print "Transfer rate " ; Lsecondselapsed ; " bytes per second"
Call Close_file()
Print
Moveafile_end:
Gosub Anykey
Return

'***************************************************************
'Test Write
'***************************************************************
'Exercise the FAT16/32 lib & mass storage lib, see if it breaks
Sub Testwrite()
Local Test_filename As String * 20 , Size_of_file As Long , Myseed As Word
Local File_byte As Byte , Rnd_wrt_cnt As Byte
Testwrite_size:
Input "size of file " , Size_of_file
If Size_of_file = 0 Then
   Print "size > 0"
   Goto Testwrite_size                                      'File size must be > than 0
End If
Testwrite_filename:
Input "filename " , Test_filename
'Illegal characters are stripped off in create_file & find_file. Also converted to upper case & implied dot taken care of.
Filenumber = Find_file(test_filename)
If Filenumber <> &HFFFF Then
   Print Test_filename ; " exists"
   Goto Testwrite_filename                                  'File name must not exist
   'OR
'   Call Delete_file(Filenumber)                               'delete the file first
End If
Filenumber = Create_file(test_filename , Attrib_archive)    'create a file
If Errorcode = File_str_empty_wrng Then
   Print "enter ";
   Goto Testwrite_filename                                  'File name must not be an empty string
End If

Call Open_file(filenumber)                                  'open the created file
Myseed = _sec                                               'set a new seed
___rseed = Myseed                                           'assign same seed
#if Clock_is_ds1307 = 1
   Gosub Getdatetime                                        'update _sec, _min _hour variables if using ds1307
#endif
Ltimestamp = Secofday()                                     'Set time stamp
Print "tst seq wrt"
While Fileoffset < Size_of_file
   File_byte = Rnd(254)                                     'get a random number from 0 to 254
   Call Write_sequential(file_byte)                         'fill file with random numbers ranging from 0 to 254
Wend
Call Close_file()                                           'close. Final fush to disk, with end of cluster chain & update dir entries & update FSinfo

Print "read"
Call Open_file(filenumber)                                  'open file again
___rseed = Myseed                                           'assign same seed
While Filestatus.endoffileflag = 0
   File_byte = Read_sequential()                            'read a byte
   If File_byte <> Rnd(254) Then                            'compare with what it should be
         Print "wrong at " ; Fileoffset
'      Exit While
   End If
Wend
Call Close_file()                                           'only update last access date
#if Clock_is_ds1307 = 1
   Gosub Getdatetime                                        'update _sec, _min _hour variables if using ds1307
#endif
Lsecondselapsed = Secelapsed(ltimestamp)                    'calc the elapsed seconds
Print "finished in " ; Lsecondselapsed
Gosub Anykey

If Filesize > 10000000 Then Exit Sub                        'if file bigger than 10M, it will take a long time
Print "tst rnd write 1"
Call Open_file(filenumber)                                  'open file again
___rseed = Myseed                                           'assign same seed
File_byte = 255
For Rnd_wrt_cnt = 0 To 199                                  '200 random writes
   If Size_of_file < &HFFFF Then                            'if less than 64k
      Fileoffset = Rnd(filesize)                            'pick a random file offset
   Else
      Fileoffset = Rnd(&Hffff)                              'if greater than 64k, then pick a random number from 0 to 64k
   End If
   Call Write_random(file_byte)                             'at random file offsets, write 255
Next Rnd_wrt_cnt
Print Rnd_wrt_cnt ; " wrong bytes"
Call Close_file()                                           'close. Final fush to disk & update dir entries.

Print "read 1"
Call Open_file(filenumber)
___rseed = Myseed                                           'assign same seed
Rnd_wrt_cnt = 0                                             'wrong counter
While Filestatus.endoffileflag = 0
   File_byte = Read_sequential()
   If File_byte <> Rnd(254) Then                            'if wrong
      If File_byte <> 255 Then                              'it must be 255
         Print "wrong at " ; Fileoffset ; " " ; File_byte   'otherwise there was an error
      End If
      Incr Rnd_wrt_cnt                                      'incr wrong counter
   End If
Wend
Call Close_file()                                           'only update last access date
Print "end random1 " ; Rnd_wrt_cnt ; " wrong bytes"
'may not equal number of random writes, as there may have been more than 1 write to the same location
Gosub Anykey

Print "tst rnd write 2"
Call Open_file(filenumber)
File_byte = Rnd(213)                                        'start count from a random number
For Fileoffset = Filesize To 0 Step -1                      'write backwards
   Call Write_random(file_byte)
   Decr File_byte                                           'decr file byte = incr file byte forwards
   If File_byte > 213 Then File_byte = 213                  'count from 0 to 213 only (no cheating)
Next Fileoffset
Call Close_file()

Print "read 2"
Call Open_file(filenumber)
File_byte = Read_sequential()                               'get first file byte
Incr File_byte                                              'incr
If File_byte > 213 Then File_byte = 0                       'file_byte = what the next byte should be

While Filestatus.endoffileflag = 0
   Filebyte = Read_sequential()                             'filebyte = what it is
   If Filebyte <> File_byte Then                            'if the read byte <> what it should be, then something is wrong
      Print "wrong " ; Fileoffset ; " " ; Filebyte ; " " ; File_byte
'      Exit Do
   End If
   Incr File_byte                                           'The next byte should be
   If File_byte > 213 Then File_byte = 0
Wend
Call Close_file()
Gosub Anykey

Print "append"
Call Open_file(filenumber)
Call Append_file()
For Size_of_file = 1 To 10000                               'add another 10k
   File_byte = Rnd(254)                                     'get a random number from 0 to 254
   Call Write_sequential(file_byte)                         'fill file with random numbers ranging from 0 to 254
Next Size_of_file
Call Close_file()
Print "end append"
Gosub Anykey
End Sub

'***************************************************************
'Test Open Path and Create Path
'***************************************************************
Sub Pathtst()
Local Path As String * 20
Input "path " , Path
Call Open_path(path , Path , Filenumber)
If Errorcode <> 0 Then
   Print "error opening path because ";
   Select Case Errorcode
      Case Open_path_wrng
         Print "segment " ; Path ; " does not exist"
         Print "creating " ; Path
         Filenumber = Create_path(path)                     'create it
         If Errorcode = Create_path_wrng Then               'we have just created a file, that must be closed
            Call Open_file(filenumber)                      'open the file
            Call Write_sequential(filebyte)                 'write at least one byte
            Call Close_file()                               'close the file
         End If
      Case Open_path_fnd_file_wrng
         Print "found a file, rather than a dir " ; Path
      Case Open_path_fnd_dir_wrng
         Print "found a dir, rather than a file " ; Path
      Case Path_str_wrng
         Print "empty path"
      Case Else
          Print "?? " ; Path                                'should never end up here
   End Select
End If
Gosub Anykey
Call Dir2ansi(filenumber)
End Sub


'***************************************************************
'Debug Dump to ANSI
'***************************************************************
$external Alpha_num_filt
Sub Debugdump2ansi(direntry As Word)
Local Hexcol As Byte , Currentrow As Byte , Filestr As String * 1 , Asciicol As Byte
Attrib = Read_dir_attr(direntry)
If Attrib.attrib_directorybit = 1 Then
   Print "Cannot dump <DIR>"
   Goto Debugdump2ansi_end
End If
Call Open_file(direntry)                                    'open the file
Print "dump " ; Read_dir_name_ext(direntry)
Currentrow = 1
While Filestatus.endoffileflag = 0
   If Inkey() = 27 Then Exit While                          'escape = bug out
   If Currentrow > Rows Then Currentrow = 1
   Call Ansilocate(currentrow , 1)                          'File Offset position
   Gosub Ansiclrline                                        'clear the whole line
   Print Fileoffset
   Hexcol = 10
   Asciicol = 63
   Do
      Filebyte = Read_sequential()                          'Read a byte sequentialy
      Filestr = Chr(filebyte)                               'turn the file byte into a string
      Filestr = Alpha_num_filt(filestr)                     'filter only alpha numeric
      Call Ansilocate(currentrow , Hexcol)
      Print Hex(filebyte) ; " " ;
      Call Ansilocate(currentrow , Asciicol)
      Print Filestr;                                        'print the file byte string
      Hexcol = Hexcol + 3
      Incr Asciicol
   Loop Until Hexcol => 58
   Incr Currentrow
Wend
Call Close_file()
Debugdump2ansi_end:
Print
Gosub Anykey
End Sub

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
