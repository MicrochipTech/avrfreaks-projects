HOW TO USE THE PC TOOLS

1. Use the GENTEMP application to create a template
   configuration file.

2. Modify the configuration file with parameters for your
   AVR device, key size, CRC checking etc.

3. Use the GCREATE application to generate the files 'aeskeys.h'
   and 'bootldr.h' to be used when building the bootloader.

4. Use the GCREATE application to encrypt your data.

5. Use the UPDATE application to communicate with the bootloader.


THE DIRECTORIES

update/		Source code for 'update'-application
create/		Source code for 'create'-application
gentemp/	Source code for 'gentemp'-application
pctools/	Binaries for the applications

