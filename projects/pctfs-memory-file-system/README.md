# pC/TFS - memory file system

Uploaded by embedded-os on 2009-02-22 20:09:00 (rating 0 out of 5)

## Summary

The Tiny-File-System, created as module for the pC/OS RTOS kernel, based on hierarchical doubly-linked lists, thereby eliminating any restriction in number of files within a directory or file size (max 4GB) is available. Each file can be in a fragmented or unfragmented form created, managed and deleted.  

The FileSystem manages all the names of directories/files as a STRING, which is also called as point symbols, spaces, and all others belong to the name of the element. However, not allowed are two points behind with one another or one or more slashes in the name.


Reserved Names elements:  

.. - one directory back  

/ - at the beginning of the path: for from ROOT  

/ - within the path: the separation of the directory / file name


The entire FileSystem works case-sensitive!  

But there are no mechanisms of the file-sharing available. That means a file can be simultaneously from multiple users open to read but only from one user to opened for writing.  

As the file attributes are Read-Only, Write-Only and Hidden (system) is available. Hidden entries are not returned in a TFS\_GetFirst .. or TFS\_GetNext .. , but can be accessed directly.  

The entire file system works on a linear memory area of a maximum of 4 GByte.


ROM: approximately 14 kBytes on a 32bit CPU  

 approximately 22 kBytes on a 8bit CPU  

RAM: approximately 24 bytes global + 16..24 bytes per user  

 + 9 bytes * TFS\_HANDLES per user  

 plus approximately 100 bytes stack


link: [http://www.embedded-os.de/index....](http://www.embedded-os.de/index.html?pctfs.htm)

## Links

- [pC/TFS - file system](http://www.embedded-os.de/index.html?pctfs.htm)

## Tags

- Complete code
