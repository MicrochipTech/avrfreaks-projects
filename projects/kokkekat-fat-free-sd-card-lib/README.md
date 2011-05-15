# KokkeKat FAT-free SD card lib

Uploaded by Kimmeridgien on 2011-05-15 13:45:00 (rating 0 out of 5)

## Summary

This application note presents my new SD card library. It is written entirely in BASCOM-AVR basic with the intention to minimize RAM usage.


What makes it special?  

â€¢ Written in commented basic so you can easily understand it and make any addition or modification you see fit.  

â€¢ Low dedicated RAM requirement â€“ read, write, and append with fsinfo updating takes only 651 bytes of RAM plus 10 bytes of $hwstack . Read-only requires only 588 bytes of RAM plus 10 bytes of $hwstack.  

â€¢ An integrated directory scrolling routine useful for forward and backward navigation applications, e.g. mp3 players.  

â€¢ Support for long filenames when scrolling and reading files.


Main features  

â€¢ Modular â€“ you can easily define which parts of the library you want to use.  

â€¢ SDSC, SDHC, and SDXC support. (For SDXC you need to format it with FAT32 in Mac OS or Linux.)  

â€¢ FAT16 and FAT32 support in root directory and subdirectory (no support for ExFAT).  

â€¢ This means that the maximum file size is 4 GB and the maximum partition size is 2 TB. It has been development tested on 32 MB SDSC, 1 GB SDSC, 4 GB SDHC, and 64 GB SDXC.  

â€¢ Find file, subdirectory, parent directory, and volume ID.  

â€¢ Read file with 8.3 or long filename.  

â€¢ Create and write to new file with 8.3 filename.  

â€¢ Create new subdirectory with 8.3 directory name.  

â€¢ Append to a pre-existing file with 8.3 filename.  

â€¢ Scroll forward or backward through a directory.  

â€¢ Update FAT32 fsinfo sector when writing.  

â€¢ Wipe out the contents while maintaining the original formatting.


Requirements  

â€¢ You must use commercial v2.0.5.0 (or higher) of BASCOM AVR. This is because this library requires the new data type DWORD: unsigned 32-bit integer.


Please download the application note and code from AN #186 at BASCOM's web site:


[http://www.mcselec.com/index.php...](http://www.mcselec.com/index.php?option=com_content&task=view&id=291&Itemid=57)


Enjoy!


Niclas, a.k.a. Kimmeridgien


Please notice that the downloads below are only the application note and circuit drawing documents. The full code can be downloaded from the link above. At [www.mcselec.com](http://www.mcselec.com) you can find the dedicated support forum for this library, which is also the place where code updates will be published.

## Compilers

- BASCOM-AVR

## Tags

- General Lib.functions
- BASCOM-AVR
