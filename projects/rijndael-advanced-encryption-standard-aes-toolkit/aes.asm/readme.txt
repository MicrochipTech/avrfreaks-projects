Rijndael / Advanced Encryption Standard (AES) Toolkit for AVR 8 bit microprocessors.

##################################
# Author: 	Michael Blenderman
# Version: 	1.1
# Date: 	5/1/2006
#
# Source: 	AVR Assembler 2.1
# Processor:	AVR AT90CAN128
##################################
____________________________________________________________________________________


1. Rijndael is the basis for AES. They are both block ciphers that use secret keys. The main difference is AES only allows 4 word blocks while Rijndael allows different block sizes like 4, 6 or 8 word blocks (these are common Rijndael block sizes). Each word is 32 bits, which is 4 bytes. Block sizes in bytes are 16, 24 and 32. Rijndael and AES both allow 4, 6 or 8 word keys. In bits this translates into 128, 192 or 256 bit keys. You should always make sure your key size is at least as large as your block size (Nk => Nb). In this software Nb indicates the number of block words and Nk indicates the number of key words. To help understand this document you should get the Rijndael and AES specifications available on the web.

2. For future reference "AES" will be used instead of "Rijndael/AES" to shorten this document. These programs are used for more than just simple text cryptology. They can also be used for MAC algorithms, hash functions, stream ciphers, pseudorandom number generators, etc. This Toolkit approach allows enough flexibility to use these programs for a variety of tasks. The drawback is it takes some knowledge of AVR assembly language to manipulate the Toolkit structure. For specific or targeted uses like AVR boot loaders, the Toolkit parts can be combined into a configurable library that does not require any assembly language knowledge to use.

3. Portability between different AVR processors. This AES Toolkit source code does not involve using timers, USARTs, ADCs, analog comparators, etc. This makes it fairly straight forward to port to another AVR chip. There are five important differences listed below.

	3a. AVR processors with more than 64k FLASH have a RAMPZ register to access FLASH above 64k. The ELPM replaces the LPM instruction when RAMPZ is used. Processors without RAMPZ do not have an ELPM instruction and use LPM instead. Because this code was written for an AT90CAN128 with 128 k of FLASH, it has RAMPZ code included in it.

	3b. AVR processors with more than 128k FLASH memory have the EIND register. This is used by the EIJMP and EICALL instructions (these processors also have the RAMPZ register). Depending on how your particular AES program implementation uses memory, you may need to change some instructions to EIJMP or EICALL.

	3c. Not all AVR chips have a hardware multiplier instruction. The AVR multiplier is used by the AES AddRoundKey() function. If you port this program to an AVR chip without the AVR hardware multiplier, then you will have to rewrite the AddRoundKey() code.

	3d. The FLASH parameters are not the same for different AVR chips. The source file boot.inc has the AT90CAN128 values, which would need to change for some other AVR chips. The key generation program FLASH writing code depends on the page size PAGE_SZ_B being 256. The aes_key_expand FLASH write code will need rewriting if this is changed.

	3e. ATMEL defined a RAMPD register for AVR chips with SRAM that exceeds 64K. This is documented in 8 bit AVR Instruction Set Rev. 0856E-AVR11/05 data sheet. At this time I cannot find any AVR chips that have a RAMPD register. The LDS and STS instructions are supposed to use RAMPD. If ATMEL ever makes an actual chip with RAMPD in it, then the Toolkit code will have to be modified if you ever want to set RAMPD to a non-zero value. If RAMPD is always kept cleared, then it should not impact the Toolkit code at all.

4. Different AVR microprocessor chips have different amounts of resources (we are interested in program FLASH size and SRAM size). In addition the AES program can be written for fastest possible execution speed or the AES program can be written to use less memory (either FLASH or SRAM). Sometimes its the available AVR chip resources alone that determine how the program is written. There are different ways of implementing AES software depending on your needs.

5. Practical limitations of AES. AES performs a different type of multiplication than the familiar binary multiplication used in most math libraries. It is called Finite Field Multiplication and is based on an irreducible polynomial of 0x11B. The AES specification shows this polynomial value as 1{1B}. There is also Inverse Finite Field Multiplication. The problem is at this time I am not aware of any program algorithm that can directly calculate the Inverse Finite Field Multiplication value in a reasonable manner. In other words rather than a simple formula, an iterative process builds up to each individual Inverse Finite Field Multiplication value which can take way too long. Instead of calculating the Inverse Finite Field Multiplication as needed, a table of all 256 values can be calculated before hand and stored in SRAM or all 256 values can be pre-calculated and stored in a FLASH table.

6. AES minimum required tables. All of the different AES functions can be efficiently implemented as program algorithms, except for the Inverse Finite Field Multiplication which needs a 256 byte table to run efficiently. However, other AES functions can also use tables for faster execution speed. Since the tables take up memory (256 bytes per table), its a tradeoff between memory and speed. All tables can be located in SRAM or in FLASH. Using the Inverse Finite Field Multiplication table is the minimum table requirement.

	6a. The Inverse Finite Field Multiplication table. The smallest memory configuration uses only this table. This table is 256 bytes and can be located in SRAM or FLASH. The AES InvSubBytes(), SubBytes() and SubWord() routines use this table (these routines must be specifically written to use this table). SubWord() is used by the key generation program. The program code that uses the Inverse Finite Field Multiplication table must make additional calculations on each table value (it's not just a simple lookup). It takes 512 bytes of SRAM to generate the 256 byte Inverse Finite Field Multiplication table, but only 256 bytes are used after the table is generated.

	6b. The Inverse Finite Field Multiplication table can be replaced with one or two tables. The SubBytes table is 256 bytes long and can be located in SRAM or FLASH. The SubBytes table is used by SubBytes() and SubWord(). The InvSubBytes table is 256 bytes long and can be located in SRAM or FLASH. The InvSubBytes table is used by InvSubBytes(). The program code that uses these tables will run faster since they only have to lookup the table value. Any tables in SRAM are calculated by the program one time when its first run. If you use these tables to replace the Inverse Finite Field Multiplication table, then this table is discarded and no longer used.

7. AES additional tables. The program code can efficiently calculate all the other required functions. However, the tables still offer slightly increased execution speed at the expense of either SRAM or FLASH memory use. The Finite Field Multiplication operation log and anti-log tables are used to build the Inverse Finite Field Multiplication table which is then used to build the SubBytes table and InvSubBytes table.

8. All the AES tables. Each table is 256 bytes. Four tables total is the maximum number needed by any complete AES program (the Inverse Finite Field Multiplication table is not needed when the SubBytes table and/or InvSubBytes table are used). However, all five tables must be generated during table creation in SRAM. In this case the Log table, Anti-log table and Inverse Finite Field Multiplication table is abandoned after table generation is completed. So, it takes up to 1280 bytes of SRAM to generate the tables, but only up to 512 bytes of SRAM are required to run all the AES programs (see 8f below for an alternative). On the other hand the tables can be loaded as constants directly into the program FLASH. Keep in mind that the number of tables needed is determined by the functions you use (encryption, decryption and key expansion).

	8a. Inverse Finite Field Multiplication table (encryption, decryption or key expansion). This table can be replaced by tables 8b and/or 8c.
	8b. SubBytes table (encryption or key expansion).
	8c. InvSubBytes table (decryption).
	8d. Log table (generating other tables only).
	8e. Anti-log table (generating other tables only).
	8f. The SRAM table generation sequence is, first the Log table and the Anti-log table are generated. The Inverse Finite Field Multiplication table is created from the Log and Anti-log tables. The Inverse Finite Field Multiplication table can overwrite the Log table. The InvSubBytes table and SubBytes table are generated from the Inverse Finite Field Multiplication table. The InvSubBytes table can overwrite the Anti-log table. If the last of the five tables generated is the SubBytes table, it can overwrite the Inverse Finite Field Multiplication table and keep the total SRAM table usage to down 512 bytes. If you do not enable encryption, then the total table size can be 256 bytes (it still takes 512 bytes to generate the table).

9. AES rounds. The AES program processes each block (the s_box[Nb]) for several rounds. The block size Nb and key size Nk determine the number of rounds Nr. There is a specified lookup table that takes Nb and Nk to lookup the number of rounds Nr. The number of rounds is 10 to 14 decimal. The number of rounds also has a big effect on the size of the expanded key schedule (see below). The AddRoundKey() function uses exactly the number of expanded key words as the block word size Nb for each round (see key lenght formula in next paragraph).

10. The AES key. You must provide the initial key (either 128, 192 or 256 bits long). The KeyExpansion() program expands your initial key into the expanded key schedule. The expanded key schedule length is Nb*(Nr+1) words long. When Nb = 4 and Nk = 8 then the number of rounds is 14. This means the expanded key schedule size for 4 word blocks with a 256 byte key is 4*(14+1) = 60 words. With 4 bytes per word that makes the expanded key schedule 240 bytes long. With Nb = 8 and Nk = 8 the expanded key schedule is 120 words (480 bytes) long (this is the maximum expanded key schedule size).

11. AES keys and minimum memory requirements. Special KeyExpansion() code is used when trying to squeeze the AES program size down to its absolute minimum. The special code only expands Nb words of the key schedule for each round as its needed. To do this the expanded key schedule must be stored in SRAM. In practice this takes two Nb sized buffers for the expanded key. This will significantly reduce the storage requirements for the expanded key. An AES 240 byte expanded key schedule buffer would reduce down to 32 bytes. One drawback is it will take allot more AVR processor cycles for each AES AddRoundKey() operation. This special version of the KeyExpansion() code is not supplied with the Toolkit, but it can be written from the supplied KeyExpansion() code. A good grasp of assembly language is needed when attempting to squeeze a program down to a critically small size. Writing an efficient small size round based KeyExpansion() program requires knowing the Nb and Nk sizes before hand. Trying to write a general purpose round based KeyExpansion() program will take extra program code, which defeats the purpose of writing it for minimum program size. This is why this particular implementation detail is left out of the Toolkit.

12. AES key security. My personal feeling is the AES key should only be placed in FLASH so it can have the protection of the memory lock bits. Theoretically SRAM is not as secure as FLASH. However, if you have to squeeze AES into a small processors FLASH then you may have no choice except to put the key in SRAM or use a different larger AVR chip. The KEY_FLASH directive is used to write an expanded key schedule into FLASH. If you do this it will still need to be written to a file then read into AVRstudio and edited.

13. There are three implementations of the toolkit. One is FLASH oriented, one is SRAM oriented and the other is a small memory footprint version. Features of the three different implementations can be combined/rearranged as needed for any particular implementation as needed.

	13a. FLASH maximized implementation. This implementation uses more bootloader FLASH. This implementation has the second fastest encryption and decryption (you will be hard pressed to measure the speed difference).
	13b. SRAM maximized implementation. This implementation uses SRAM for the tables. It has extra program code to generate the tables, but this extra code takes less FLASH memory than the tables would use. This implementation has the fastest encryption and decryption.
	13c. Small memory footprint implementation. Wherever possible this implementation uses program algorithms instead of tables (the Inverse Finite Field Multiplication table is the only one required). It will run slightly slower than the two above implementations, but it can be placed in some AVR chips with smallest possible bootloader memory sizes that can fit AES. The final memory usage reduction step which is not included is to write the KeyExpansion() code to only expand Nb words of the key schedule as needed for each round (it reduces the amount of memory requires to store the expanded key schedule). The reason it is not included is given above in section 11.

14. Here is how the optimization of the MixColumns and InvMixColumns is broken down. The r_row values are simply result accumulators for the i_row values. The i_row values are copied from the state box (s_box [row, column]). In the examples below the shorthand ( < 1) is used to represent a GF(256) Finite Field Multiplication times 2 (one shift to the left). Of course the eighth order irreducible polynomial 1{1B} must be added (exclusive ORed) if there is a carry from the shift. The ( < 2) is GF(256) times four (two shifts to the left) and ( < 3) is GF(256) times eight (three shifts to the left). Using the repeated shifts method to multiply the GF(256) column with the i_row value you only need to shift for every 1 value bit. So, {0B} has bit 0 = 1, bit 1 = 1, bit 2 = 0, bit 3 = 1 and bits 4 through 7 are all = 0. Bit 0 is a special case where just the original i_row value is used without any shifting. This means (i_row * {0B}) in GF(256) = i_row ^ (i_row < 1) ^ (i_row < 3). A general purpose multiply by GF(256) for two unknown 8 bit values always takes 8 cycles (7 shifts total). Performing the unneeded general purpose GF(256) multiply cycles makes a huge increase in the number of CPU cycles needed by the program. The optimization below really speeds things up by a huge factor.


Refer to page 18 of the FIPS PUB 197 section 5.1.3.

MixColumns (each column is composed of 4 rows):

GF(256) ||   r_row0   |   r_row1   |   r_row2   |   r_row3   |
--------++------------+------------+------------+------------+
--------++------------+------------+------------+------------+
  {02}  || i_row0 < 1 | i_row1 < 1 | i_row2 < 1 | i_row3 < 1 | {02} bit 1 = 1
--------++------------+------------+------------+------------+
  {03}  || i_row1     | i_row2     | i_row3     | i_row0     | {03} bit 0 = 1
        || i_row1 < 1 | i_row2 < 1 | i_row3 < 1 | i_row0 < 1 | {03} bit 1 = 1
--------++------------+------------+------------+------------+
  {01}  || i_row2     | i_row3     | i_row0     | i_row1     | {01} bit 0 = 1
--------++------------+------------+------------+------------+
  {01}  || i_row3     | i_row0     | i_row1     | i_row2     | {01} bit 0 = 1
--------++------------+------------+------------+------------+

r_row0 = i_row1 ^ i_row2 ^ i_row3
r_row1 = i_row0 ^ i_row2 ^ i_row3
r_row2 = i_row0 ^ i_row1 ^ i_row3
r_row3 = i_row0 ^ i_row1 ^ i_row2

r_row0 ^= (i_row0 < 1) ^ (i_row1 < 1)
r_row1 ^= (i_row1 < 1) ^ (i_row2 < 1)
r_row2 ^= (i_row2 < 1) ^ (i_row3 < 1)
r_row3 ^= (i_row0 < 1) ^ (i_row3 < 1)


Refer to page 23 of the FIPS PUB 197 section 5.3.3.

InvMixColumns (each column is composed of 4 rows):

GF(256) ||   r_row0   |   r_row1   |   r_row2   |   r_row3   |
--------++------------+------------+------------+------------+
--------++------------+------------+------------+------------+
  {0E}  || i_row0 < 1 | i_row1 < 1 | i_row2 < 1 | i_row3 < 1 | {0E} bit 1 = 1
        || i_row0 < 2 | i_row1 < 2 | i_row2 < 2 | i_row3 < 2 | {0E} bit 2 = 1
        || i_row0 < 3 | i_row1 < 3 | i_row2 < 3 | i_row3 < 3 | {0E} bit 3 = 1
--------++------------+------------+------------+------------+
  {0B}  || i_row1     | i_row2     | i_row3     | i_row0     | {0B} bit 0 = 1
        || i_row1 < 1 | i_row2 < 1 | i_row3 < 1 | i_row0 < 1 | {0B} bit 1 = 1
        || i_row1 < 3 | i_row2 < 3 | i_row3 < 3 | i_row0 < 3 | {0B} bit 3 = 1
--------++------------+------------+------------+------------+
  {0D}  || i_row2     | i_row3     | i_row0     | i_row1     | {0D} bit 0 = 1
        || i_row2 < 2 | i_row3 < 2 | i_row0 < 2 | i_row1 < 2 | {0D} bit 2 = 1
        || i_row2 < 3 | i_row3 < 3 | i_row0 < 3 | i_row1 < 3 | {0D} bit 3 = 1
--------++------------+------------+------------+------------+
  {09}  || i_row3     | i_row0     | i_row1     | i_row2     | {09} bit 0 = 1
        || i_row3 < 3 | i_row0 < 3 | i_row1 < 3 | i_row2 < 3 | {09} bit 3 = 1
--------++------------+------------+------------+------------+

r_row0 = i_row1 ^ i_row2 ^ i_row3
r_row1 = i_row0 ^ i_row2 ^ i_row3
r_row2 = i_row0 ^ i_row1 ^ i_row3
r_row3 = i_row0 ^ i_row1 ^ i_row2

r_row0 ^= (i_row0 < 1) ^ (i_row1 < 1)
r_row1 ^= (i_row1 < 1) ^ (i_row2 < 1)
r_row2 ^= (i_row2 < 1) ^ (i_row3 < 1)
r_row3 ^= (i_row0 < 1) ^ (i_row3 < 1)

r_row0 ^= (i_row0 < 2) ^ (i_row2 < 2)
r_row1 ^= (i_row1 < 2) ^ (i_row3 < 2)
r_row2 ^= (i_row0 < 2) ^ (i_row2 < 2)
r_row3 ^= (i_row1 < 2) ^ (i_row3 < 2)

r_row0 ^= (i_row0 < 3) ^ (i_row1 < 3) ^ (i_row2 < 3) ^ (i_row3 < 3)
r_row1 ^= (i_row0 < 3) ^ (i_row1 < 3) ^ (i_row2 < 3) ^ (i_row3 < 3)
r_row2 ^= (i_row0 < 3) ^ (i_row1 < 3) ^ (i_row2 < 3) ^ (i_row3 < 3)
r_row3 ^= (i_row0 < 3) ^ (i_row1 < 3) ^ (i_row2 < 3) ^ (i_row3 < 3)

Each complete r_row column in the above tables has all the operations required for that row. For clarity the results are divided up according to the number of GF(256) shifts and sorted into numerical row order. This is the how and why that explains the arcane looking operations in the optimized MixColumns and InvMixColumns code.
