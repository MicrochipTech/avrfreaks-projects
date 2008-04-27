# AVRCryptoLib

Uploaded by emilevanderlaan on 2008-04-26 17:54:00 (rating 0 out of 5)

## Summary

Now released the of the AVRCryptoLib for the GNU C compiler.  

It contains DES, Triple DES, and RSA, AES, SkipJack, MD5 and SHA-1  

Documentation and Source code is included and some test programs.  

The interfacing to function is in C but all the main functions are in assembly language for speed and size.  

The DES will do a decryption in 8.3ms @16Mc Mega Core.  

And 512^3 RSA in 69ms @16mc on a Mega core 512^512 in 26 sec.  

You will need to register to get all the source code  

<http://www.emsign.nl/>  

Scroll down for the speed/codesize table




|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| DES
  Cycles
 Code size
 Ram needed
|  Encrypt
 133966
 1782 bytes
 122+24 bytes
|  Decrypt
 134134
 1780 bytes
 122+24 bytes
|  Combined
  2082 bytes
 122+24 bytes
| Triple Des
  Cycles
 Code size
 Ram needed
|  Encrypt
 402568
 1974 bytes
 122+32 bytes
|  Decrypt
 402688
 1974 bytes
 122+32 bytes
|  Combined
  2390 bytes
 122+32 bytes
| RSA 64 bits Key in Ram
  Cycles
 Code size
 Ram needed
|  Encrypt
 1051026
 760 bytes
 5 * Keysize bytes
|  Decrypt
 22946
 690 bytes
 4 * Keysize bytes
|  Conbined
  1040 bytes
 5 * Keysize bytes
| RSA 64 Key in Flash
  Cycles
 Code size
 Ram needed
|  Encrypt
 1118827
 746 bytes
 4 * Keysize bytes
|  Decrypt
 24660
 692 bytes
 3 * Keysize bytes
|  Combined
 -
 1028 bytes
 4 * Keysize bytes
| AES 128 SF
  Cycles
 Code size
 Ram needed
|  Init
 25389
|  Encrypt
 7622
 900 bytes
 768+16 bytes
|  Decrypt
 8279
 904 bytes
 768+16 bytes
|  Combined
  1302 bytes
 768+16 bytes
| AES 128 SR
  Cycles
 Code size
 Ram needed
|  Init
 5534
|  Encrypt
 7762
 966 bytes
 256+16 bytes
|  Decrypt
 8411
 1222 bytes
 256+16 bytes
|  Combined
  1620 bytes
 256+16 bytes
| SkipJack
  Cycles
 Code size
 Ram needed
|  Encrypt
 5777
 526 bytes
 18 bytes
|  Decrypt
 5727
 520 bytes
 18 bytes
|  Combined
 -
 790 bytes
 18 bytes
| MD5
  Cycles
 Code size
 Ram needed
|  Block 1KBytes
 339501
 1842 bytes
 113 bytes
| SHA-1
  Cycles
 Code size
 Ram needed
|  Block 1KBytes
 489890
 1640 bytes
 120 bytes | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |
 | | | | |

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
