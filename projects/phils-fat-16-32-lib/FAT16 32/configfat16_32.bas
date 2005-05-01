'Configuration for FAT16 FAT32

$lib "fat16_32.lbx"                                         'The Library
$lib "utilities.lbx"                                        'Extra general purpose routines

Const Fatbuffersectorsize = 1                               'The size of the FAT buffer in sectors is?
'Filebuffersectorsize must be a base 2 number, that is less than or equal to sectors per cluster. Make it 1 for MAX compatability.
Const Filebuffersectorsize = 1                              'The size of the file buffer in sectors is?

'dir_attrib bytes
Const Attrib_read_only = &H01
Const Attrib_hidden = &H02
Const Attrib_system = &H04
Const Attrib_volume_id = &H08
Const Attrib_directory = &H10
Const Attrib_archive = &H20
Const Attrib_long_name = &H0F
'A valid file is?
Const Attrib_valid = Attrib_read_only Or Attrib_hidden Or Attrib_system Or Attrib_directory Or Attrib_archive
Const Kilo = 1024                                           '1k is?
Const Filebuffersize = Filebuffersectorsize * 512
'Dim Filebuffer(filebuffersize) As Xram Byte                 'The place where all the file stuff goes
Dim Filebuffer(filebuffersize) As Byte                      'The place where all the file stuff goes
Const Fatbuffersize = Fatbuffersectorsize * 512
'Dim Fatbuffer(fatbuffersize) As Xram Byte                   'The place where all FAT stuff goes
Dim Fatbuffer(fatbuffersize) As Byte                        'The place where all FAT stuff goes

'Subs & Functions & Routines used
#if Media_is_rotating = 1
   $external Sectic
#endif

Declare Sub Random_access()
$external Random_access
Declare Sub Append_file()
$external Append_file
Declare Sub Close_file()
$external  Close_file
Declare Sub Initfat()
$external Initfat
Declare Function Read_sequential() As Byte                  'Read a byte sequentially
$external Read_sequential
Declare Function Read_16()as Word                           'Read a word
$external Read_16
Declare Function Read_32()as Long                           'Read a long
$external Read_32
Declare Function Read_random() As Byte                      'Read a byte randomly (set global "Fileoffset" prior)
$external Read_random
Declare Function Read_dir_name(direntry As Word) As String  'Read the name of the file
$external Read_dir_name
Declare Function Read_dir_ext(direntry As Word) As String   'Read the extension of the file
$external Read_dir_ext
Declare Function Read_dir_name_ext(direntry As Word) As String       'Read the name.ext of the file
$external Read_dir_name_ext
Declare Function Read_dir_firstcluster(direntry As Word) As Long       'Read the first cluster of the file
$external Read_dir_firstcluster
Declare Function Read_dir_filesize(direntry As Word) As Long       'Read the file size of the file
$external Read_dir_filesize
Declare Function Read_dir_attr(direntry As Word) As Byte    'Read the attribute of a file
$external Read_dir_attr
Declare Function Read_dir_wrttime(direntry As Word) As String       'Read the write time of the file
$external Read_dir_wrttime
Declare Function Read_dir_wrtdate(direntry As Word) As String       'Read the write date of the file
$external Read_dir_wrtdate
Declare Function Read_dir_crttime(direntry As Word) As String       'Read the create time of the file
$external Read_dir_crttime
Declare Function Read_dir_crtdate(direntry As Word) As String       'Read the create date of the file
$external Read_dir_crtdate
Declare Function Read_dir_lstaccdate(direntry As Word) As String       'Read the last access date of the file
$external Read_dir_lstaccdate
Declare Sub Write_dir_name(dir_name As String , Direntry As Word)       'Write the name of the file
$external Write_dir_name
Declare Sub Write_dir_ext(dir_ext As String , Direntry As Word)       'Write the extension of the file
$external Write_dir_ext
Declare Sub Write_dir_name_ext(name_dot_ext As String , Direntry As Word)       'Write the name.ext of the file
$external Write_dir_name_ext
Declare Sub Write_dir_filesize(direntry As Word)            'Write the current file offset of the file
$external Write_dir_filesize
Declare Sub Write_dir_attr(attribute As Byte , Direntry As Word)       'Write the attribute of a file
$external Write_dir_attr
Declare Sub Write_dir_wrtdate(direntry As Word)             'Write the current date to the file
$external Write_dir_wrtdate
Declare Sub Write_dir_crtdate(direntry As Word)             'Write the current date to the file
$external Write_dir_crtdate
Declare Sub Write_dir_lstaccdate(direntry As Word)          'Write the current date to the file
$external Write_dir_lstaccdate
Declare Sub Write_dir_wrttime(direntry As Word)             'Write the current time to the file
$external Write_dir_wrttime
Declare Sub Write_dir_crttime(direntry As Word)             'Write the current time to the file
$external Write_dir_crttime
Declare Sub Write_dir_firstcluster(firstcluster As Long , Direntry As Word)
$external Write_dir_firstcluster
Declare Sub Open_file(direntry As Word)                     'Open the file or dir
$external Open_file
Declare Sub Open_path(path As String , Remain_path As String , Direntry As Word)       'Open a whole path
$external Open_path
Declare Function Find_file(dir_name_ext As String) As Word  'Find a file that match
$external Find_file
Declare Sub Next_file(direntry As Word , Byval Updown As Byte)       'Find the next or previous valid file
$external Next_file
'See if the file matches the file_mask or Attrib_mask
Declare Function Match_dir_name_ext_attr(file_mask As String , Byval Attrib_mask As Byte , Direntry As Word) As Byte
$external Match_dir_name_ext_attr
Declare Sub Write_sequential(file_byte As Byte)             'Write a byte sequentially
$external Write_sequential
Declare Sub Write_16(file_word As Word)                     'Write a word
$external Write_16
Declare Sub Write_32(file_long As Long)                     'Write a long
$external Write_32
Declare Sub Write_random(file_byte As Byte)                 'Write a byte randomly (set global "Fileoffset" prior)
$external Write_random
Declare Function Drivemodel() As String                     'Read the drive model
$external Drivemodel
Declare Function Find_freefile() As Word                    'Find an unsed directory entry
$external Find_freefile
Declare Function Create_file(name As String , Byval Attribute As Byte) As Word       'Create a file or dir
$external Create_file
Declare Function Create_path(path As String) As Word        'Create a whole path
$external Create_path
Declare Sub Delete_file(direntry As Word)                   'Delete a file
$external Delete_file
'Find the next or previous file that matches the file_mask or Attrib_mask, starting from Direntry
Declare Function Next_file_match(file_mask As String , Byval Attrib_mask As Byte , Direntry As Word , Byval Updown As Byte)as Word
$external Next_file_match
Declare Function Read_dir_filesizestr(direntry As Word) As String       'String of file size rounded to the nearest kilo.
$external Read_dir_filesizestr

'variables used by fat16_32.lbx
'Some must be dimentioned in specific order, do not swap about
Dim Plbabegin As Long                                       'Partition LBA Begin
Dim Bpb_secperclus As Byte                                  'Sectors per Cluster. Also referred to as "Allocation Unit"
Dim Bpb_resvdseccnt As Word                                 'Bios Parameter Block Reserved Sector Count.
Dim Bpb_numfats As Byte                                     'The count of FAT data structures on vol. Usually 2
'Bpb_totsec is only used once to calculate Cntofclusters. TO DO LIST remove & save 4 bytes
Dim Bpb_totsec As Long                                      'The total count of sectors on the vol.
Dim Bpb_fatz As Long                                        'Bios Parameter Block Size (per one FAT table)
Dim Bpb_rootclus As Long                                    'Bios Parameter Block Root Cluster (FAT32 only). Usually 2
Dim Bpb_fsinfo As Long                                      'Bios Parameter Block File System Info sector (FAT32 only). Usually 1 + Plbabegin
Dim Cntofclusters As Long                                   'Count of (valid) Clusters on volume. Calculated from Bpb_totsec & Bpb_secperclus & Dirlbabegin
Dim Dirlbabegin As Long                                     'Dir LBA Begin. Calculated from Fatlbabegin & Bpb_fatz
Dim Root_dir_sectors As Byte                                'Count of sectors occupied by a FAT16 root directory
Dim Bytesperclus As Word                                    'Bytes Per Cluster. Calculated from Bpb_bytespersec & Bpb_secperclus.
Dim Fatlbabegin As Long                                     'FAT LBA Begin (1st fat). Calculated from Plbabegin & Bpb_resvdseccnt
Dim Free_count As Long                                      'Last known free cluster count
Dim Nxt_free As Long                                        'Last known first free cluster
Dim Fatloadedstart As Long                                  'contains the first fat sector loaded
Dim Filenumofit As Byte                                     'The number of sector iterations to fill buffer
Dim Fatstatus As Byte                                       'Current fat status
Dim Errorcode As Byte                                       'Error codes, also used by mass storage driver.

'*****************************
'Next 5 must be in order******
Dim Dircluster As Long                                      'The current dir cluster
Dim Dirstatus As Byte                                       'bit 7 = End Of File & bits 0 to 6 = the current count of sector iterations
Dim Dirclusterindex As Word                                 'The current dir cluster index
Dim Dirfirstcluster As Long                                 'The first dir cluster of the dir
Dim Diroffset As Long                                       'directory byte counter. Base 0
'Offsets Dircluster=0-3, dirstatus=4, Dirclusterindex=5-6, Dirfirstcluster=7-10, Diroffset=11-14
'*****************************
'Next 7 must be in order******
Dim Filecluster As Long                                     'The current file cluster
Dim Filestatus As Byte                                      'bit 7 = End Of File & bits 0 to 6 = the current count of sector iterations
Dim Fileclusterindex As Word                                'The current file cluster index
Dim Filefirstcluster As Long                                'The first file cluster of the file
Dim Fileoffset As Long                                      'File byte counter. Base 0
Dim Filesize As Long                                        'The size of the file
Dim Fileparentfilenumber As Word                            'The parent file file number
'Offsets Filecluster=0-3, filestatus=4, Fileclusterindex=5-6, Filefirstcluster=7-10, Fileoffset=11-14
'Filesize=15-18 ,Fileparentfilenumber=19-22
'*****************************

'Constants used
Const Kilo_lo = Kilo And &HFF
Const Kilo_hi =(kilo And &HFF00) \ &H100

Const Filebuffersize_lo = Filebuffersize And &HFF
Const Filebuffersize_hi =(filebuffersize And &HFF00) / &H100

Const Filebuffersizeminus1 = Filebuffersize - 1
Const Filebuffersizeminus1_lo = Filebuffersizeminus1 And &HFF
Const Filebuffersizeminus1_hi =(filebuffersizeminus1 And &HFF00) / &H100

'fat status bits
.equ Filebuffermodifiedflag = 7                             '1 = File buffer is modified
.equ Randommode = 6                                         '1 = Random mode
.equ Fatbuffermodifiedflag = 5                              '1 = FAT buffer is modified
.equ Fatis16 = 4                                            '1 = fat is FAT16, 0 = FAT is FAT32

'.equ Bytesperbyte = 1
'.equ Bytesperword = 2
'.equ Bytesperlong = 4
'.equ Bytespersec = 512
'.equ Bytespersec_lo = Bytespersec And &HFF
'.equ Bytespersec_hi =(bytespersec And &HFF00) / &H100
'.equ Bytesperdirentry = 32
'.equ Signature = &HAA55
'.equ Signature_lo = Signature And &HFF
'.equ Signature_hi =(signature And &HFF00) / &H100
'dir_attrib bits
.equ Attrib_read_onlybit = 0
.equ Attrib_hiddenbit = 1
.equ Attrib_systembit = 2
.equ Attrib_volume_idbit = 3
.equ Attrib_directorybit = 4
.equ Attrib_archivebit = 5
'file status or dir status bits
.equ Endoffileflag = 7                                      '1 = end of file
'error codes
'0 = no error
'1 to 20 reserved for fatal ata driver or card driver
'21 to 40 reserved for fatal fat driver
'41 to 60 reserved for non fatal fat driver
'Fatal errors
.equ Sig_chk_err = 21                                       'Failed Master Boot Record, or Bios Parameter Block, or FSI signature check
.equ Fatis12_err = 22                                       'Fat12 is not supported
.equ Nxt_clus_eoc_err = 23                                  'Attempt to look for the next cluster while at end of cluster chain
.equ Fill_buff_eof_err = 24                                 'Attempt to fill buffer while at end of file
.equ Fat_chain_err = 25                                     'Next cluster contains an un allocated cluster.
.equ Disk_full_err = 26                                     'Disk is full
.equ Flush_empty_buff_err = 27                              'attempt to flush an empty buffer in random mode
.equ Append_no_eoc_err = 28                                 'append did not find an end of cluster chain
.equ Bytes_per_sec_err = 29                                 'Bios Paramter Block Bytes per Sector not equal to 512
.equ No_fat_loaded_err = 30                                 'attempt to force a fresh fat buffer, while fat bufer is modified
.equ Open_file_err = 31                                     'attempt to open a volume ID or a long file name
'non fatal errors
.equ Del_dir_wrng = 41                                      'cannot delete directory
.equ Open_path_wrng = 42                                    'Open_path unable to open the whole path
.equ Open_path_fnd_file_wrng = 43                           'Open_path found a file rather than a directory
.equ Open_path_fnd_dir_wrng = 44                            'Open_path found a directory rather than a file
.equ Create_path_wrng = 45                                  'Create_path created a file, that must be closed
.equ Path_str_wrng = 46                                     'Path string is empty
.equ File_str_empty_wrng = 47                               'File name is empty