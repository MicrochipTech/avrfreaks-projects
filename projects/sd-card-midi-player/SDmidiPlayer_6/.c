
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "SPI.h"
#include "LCD.h"
#include "SDread.h"
#include "IRremote.h"

// +-----------------------------+
// | SD Card Wiring:             |
// +---------+-------+-----------+
// | ATmega  |  Pin  |  SD_Card  |
// +---------+-------+-----------+
// | SS      |  PB2  |  CS       |
// | SCK     |  PB5  |  SCLK     |
// | MOSI    |  PB3  |  DI       |
// | MISO    |  PB4  |  DO       |
// +---------+-------+-----------+

//  SD Selection

#define  SDselect()    PORTB &= ~_BV(PORTB2)   // CS = L
#define  SDunselect()  PORTB |=  _BV(PORTB2)   // CS = H

//  Definitions for MMC/SDC command

#define  CMD0   (0x40+ 0)    // GO_IDLE_STATE     
#define  CMD1   (0x40+ 1)    // SEND_OP_COND (MMC)
#define ACMD41  (0xC0+41)    // SEND_OP_COND (SDC)
#define  CMD8   (0x40+ 8)    // SEND_IF_COND      
#define  CMD16  (0x40+16)    // SET_BLOCKLEN      
#define  CMD17  (0x40+17)    // READ_SINGLE_BLOCK 
#define  CMD24  (0x40+24)    // WRITE_BLOCK       
#define  CMD55  (0x40+55)    // APP_CMD           
#define  CMD58  (0x40+58)    // READ_OCR          

#define  HCS    (1UL<<30)    // High Capacity selected

uint8_t  SDbuffer[512];       // SD card read buffer (1 block)
uint32_t ActualBlock;         // Actual BLock Loaded in SDbuffer
uint8_t  BlockAddressing = 0; // or !0 for High Capacity Cards -> blocks of 512 bytes

// FAT data
uint32_t PartStart;    // Partition Start LBA
uint8_t  nbFAT;        // Number of FATs
uint32_t FATstart;     // Start of FAT LBA
uint32_t FATsize;      // Size of each FAT (clusters)
uint32_t ResSect;      // Number of reserved Sectors
uint32_t ClusterStart; // First Cluster LBA
uint32_t ClusterSize;  // Size of Clusters (in blocks=512bytes)
uint32_t RootClust;    // Root Dir First Cluster

// Actual Directory data
DirStruct Dir;

// Memories
DirStruct Memory[10];

// Actual File data
FileStruct File;

// Long Name positions
const uint8_t lona[13] = { 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30 };

void LCD_hex( uint8_t code )
{
  char v[4];
  itoa(code,v,16); 
  if( v[1]==0 ) LCD_putc( '0' );
  LCD_putc( v[0] & 0x7F );
  if( v[1]!=0 ) LCD_putc( v[1] & 0x7F );
}

void debug( uint8_t line, char *txt, uint8_t code )
{
  char v[4];
  itoa(code,v,16); 
  LCD_move(line,0);
  LCD_puts(txt);
  LCD_putc(' ');
  LCD_puts(v);
}


// Send a command to the SD Card
uint8_t SDcmd( uint8_t cmd, uint32_t arg )
{
  uint8_t CRC, res, i;
  if( cmd & 0x80)   // ACMD<n> is the command sequense of CMD55-CMD<n>
  {
    cmd &= 0x7F;
    res = SDcmd( CMD55, 0 );
    if( res & 0xFE ) return res;
  }
  // Start transaction : Pulse CS H->L
  SDunselect();
  spi_read();
  SDselect();
  spi_read();
  // Send a command + 4 Arguments
  spi_write(cmd);
  spi_write((uint8_t)(arg >> 24));
  spi_write((uint8_t)(arg >> 16));
  spi_write((uint8_t)(arg >> 8));
  spi_write((uint8_t)arg);
  // Dummy CRC
  CRC = 0x01;                           
  if( cmd == CMD0) CRC = 0x95; // Valid CRC for CMD0(0)
  if( cmd == CMD8) CRC = 0x87; // Valid CRC for CMD8(0x1AA)
  spi_write(CRC);
  // Wait for a valid response in timeout of 10 attempts
  i = 10; 
  do { res = spi_read(); } while( (res & 0x80) && --i);
  // Return the response value
  return res;
}


// Initialise the SD Card
uint8_t SDinit(void)
{
  uint8_t  OCR[4], res, j;
  uint16_t i;
  BlockAddressing = 0;
  // put SDcard in SPI mode
  spi_init();
  // 80 clocks with CS=H
  SDunselect();
  for( uint8_t j=0; j<10; j++ ) spi_read();
  SDcmd(CMD0, 0);
  // GO_IDLE_STATE
  if( SDcmd(CMD0, 0) != 0x01 )
    res = SDnodisk;
  else
  {
    // LCD_line(2,"Disk found...");
    // Read OCR
    if( SDcmd(CMD8, 0x1AA) != 0x01 )
    {
      //debug(1,"SD CMD8 failed.", res);
      res = SDerror; 
    }
    else
    {
      for( j=0; j<4; j++ ) OCR[j] = spi_read(); // Get OCR
      // Check Voltage range
      if( OCR[2]!=0x01 || OCR[3]!=0xAA )
      {
        //debug(1,"SD Voltage range error", OCR[2]);
        res = SDerror;
      }
      else
      {
        // Wait for leaving idle state (ACMD41 with HCS bit)
        for( i=10000UL; i && SDcmd(ACMD41, HCS); i-- ) _delay_us(100);
        if( i==0 )
        {
          //debug(1,"SD ACMD41 Time out", res);
          res = SDtimeout; 
        }
        else
        {
          res = SDcmd(CMD58, 0); // READ_OCR
          if( res != 0x00 )
          {
            //debug(1,"SD CMD58 Error", res);
            return SDerror;
          }
          else 
          {
            for( i=0; i<4; i++ ) OCR[i] = spi_read();
            if( OCR[0] & 0x40 )
            {
              BlockAddressing = !0; 
              //LCD_line(1,"SD v2 High Capacity");
            }
            else
            { 
              //LCD_line(1,"SD v2 Low Capacity");
            }
            // Set R/W block length to 512 for FAT
            res = SDcmd(CMD16, 512UL);
            if( res != 0x00 ) 
            {
              //debug( 1,"CMD16 failed", res );
              res = SDerror;
            }
            else
            {
              // Set SPI at maximum speed
              spi_max();
              res = SDok;
            }
          }      
        }
      }
    }
  }
  SDunselect();
  spi_read();
  return res;
}


// Read one block at "address" in SDbuffer 
uint8_t SDread( uint32_t address ) 
{
  uint8_t  dataToken;
  uint16_t i;
  uint8_t  res;
  if( address == ActualBlock ) return SDok; // Already Loaded
  ActualBlock = address;
  // Convert to byte address if needed
  if( !BlockAddressing ) address *= 512UL; 
  // READ_SINGLE_BLOCK
  res = SDcmd(CMD17, address);
  if( res != 0x00 ) 
  {
    //debug( 1,"CMD17 failed", res );
    res = SDerror;
  }
  else
  {
    // Wait for data packet
    i = 40000;
    do { dataToken = spi_read(); } while( dataToken == 0xFF && --i );
    if( dataToken != 0xFE )
    {
      //debug( 1,"CMD17 timeout", res );
      res = SDtimeout;
    }
    else
    {
      // A data packet arrived
      for( i=0; i<512; i++ ) SDbuffer[i] = spi_read();
      spi_read(); // Skip the CRC
      res = SDok;
    }
  }
  SDunselect();
  spi_read();
  return res;
}


// Read one block at "Pos" in SDbuffer 
uint8_t SDloadBuffer( PosStruct *Pos )
{
  uint8_t res;
  uint32_t dBlock = ClusterStart + ( Pos->Cluster - 2UL ) * ClusterSize + Pos->Block;
  res = SDread( dBlock );
  if( res != SDok )
  {
    LCD_line(0,"Read SD failed");
    return SDerror;
  }
  return SDok;
}

// Print partition details
void SDpartitionPrint( uint8_t i, uint8_t offset )
{
  LCD_cls();     LCD_puts("Partition ");  LCD_hex(i);
  LCD_move(1,0); LCD_puts("Boot Flag ");  LCD_hex(SDbuffer[offset+0]);
  LCD_move(2,0); LCD_puts("Type Code ");  LCD_hex(SDbuffer[offset+4]);
  LCD_move(3,0); LCD_puts("Part Start "); LCD_print32( PartStart, 16 );
  while( IRcode() != LG_Enter ) _delay_ms(100);
}


// Print Boot sector details
void SDbootPrint(void)
{
  uint8_t i;
  for( i=71; i<82; i++ ) LCD_putc( SDbuffer[i] );
  LCD_puts(" N.FAT "); LCD_hex( nbFAT );
  LCD_move(1,0);
  LCD_puts("Block "); LCD_print32( (uint32_t)SDbuffer[12] << 8 | (uint32_t)SDbuffer[11], 10 );
  LCD_puts(" Clust "); LCD_print32( ClusterSize, 10 );
  LCD_move(2,0);
  //LCD_puts("Res.Sec "); LCD_print32( ResSect, 10 );
  LCD_puts("FAT size "); LCD_print32( FATsize, 10 );
  LCD_move(3,0);
  LCD_puts("RootC "); LCD_print32( RootClust, 16 );
  while( IRcode() != LG_Enter ) _delay_ms(100); // PAUSE
}


// Read Boot Sector
uint8_t SDboot(void) 
{
  uint8_t res, i;
  uint16_t offset=0x1BE; // Start of Partition data 
  // READ PARTITION TABLE
  ActualBlock = 1; // Force SDread
  res = SDread(0);
  if( res != SDok || SDbuffer[510] != 0x55 || SDbuffer[511] != 0xAA ) 
  {
    LCD_line(0,"No Partition table");
    for(;;);
    return SDbadformat;
  }
  i=0;
  do
  {
    PartStart = (uint32_t)SDbuffer[offset+11]<<24 | (uint32_t)SDbuffer[offset+10]<<16 | (uint32_t)SDbuffer[offset+9]<<8 | (uint32_t)SDbuffer[offset+8];
    //SDpartitionPrint( i, offset );
    offset += 16;
    i++;
  }
  while( i<4 && PartStart==0 );
  // READ MASTER BOOT RECORD
  LCD_cls();
  SDbuffer[510] = 0;
  SDbuffer[511] = 0;
  res = SDread(PartStart);
  if( res != SDok )
  {
    LCD_line(0,"MBR start failed");
    for(;;);
    return SDbadformat;
  }
  if( SDbuffer[510] != 0x55 || SDbuffer[511] != 0xAA )
  {
    LCD_line(0,"MBR bad signature");
    for(;;);
    return SDbadformat;
  }
  if( SDbuffer[12] != 0x02 || SDbuffer[11] != 0x00 )
  {
    LCD_line(0,"Sector Size != 512");
    for(;;);
    return SDbadformat;
  }
  nbFAT = SDbuffer[16];
  ClusterSize =  SDbuffer[13];
  ResSect   = (uint32_t)SDbuffer[15]<<8 |(uint32_t)SDbuffer[14]; // Reserved Sectors
  FATsize   = (uint32_t)SDbuffer[39]<<24|(uint32_t)SDbuffer[38]<<16|(uint32_t)SDbuffer[37]<<8|(uint32_t)SDbuffer[36];
  RootClust = (uint32_t)SDbuffer[47]<<24|(uint32_t)SDbuffer[46]<<16|(uint32_t)SDbuffer[45]<<8|(uint32_t)SDbuffer[44];
  FATstart = PartStart + ResSect;
  ClusterStart = FATstart + nbFAT * FATsize;
  // SET ACTUAL DIR TO ROOT DIRECTORY
  Dir.Pos.Start = RootClust;
  Dir.Pos.Cluster = Dir.Pos.Start;
  Dir.Pos.Block = 0;
  Dir.Pos.Byte = 0;
  // Display
  // SDbootPrint();
  return SDok;
}


// Read following cluster in the FAT table
uint32_t readFAT( uint32_t Cluster )
{
  uint32_t cblock, cpos;
  cblock = Cluster * 4UL / 512UL;
  cpos = Cluster * 4UL  - cblock * 512UL;
  SDread( FATstart + cblock );
  return (uint32_t)SDbuffer[cpos+3]<<24|(uint32_t)SDbuffer[cpos+2]<<16|(uint32_t)SDbuffer[cpos+1]<<8|(uint32_t)SDbuffer[cpos];
}


// Find the cluster number "Cluster_Number" (starting at 0)
void GotoCluster( PosStruct *Pos, uint16_t Cluster_Number )
{
  Pos->Cluster = Pos->Start;
  for( uint16_t i=0; i<Cluster_Number ; i++ ) Pos->Cluster = readFAT( Pos->Cluster );
  Pos->Block = 0;
  Pos->CCount = Cluster_Number;
}


// Print Directory Entry details
void SDdirPrint(void)
{
  LCD_cls();
  LCD_puts( File.Name );
  LCD_line(1,File.Short); // Short File Name + Ext
  if( File.Attr & ATTR_DELETED ) LCD_puts( " Deleted" ); // Free Entry
  LCD_move(2,0);
  LCD_puts("Entry "); LCD_hex( Dir.Entry ); // Entry position (bug si CCount>0)
  if( File.Attr == ATTR_LONG_NAME )
  {
    LCD_puts(" Long ");
    LCD_hex( SDbuffer[Dir.Pos.Byte] );
  }
  else
  {
    LCD_puts(" Attr ");
    uint8_t Code = '-';
    if( File.Attr & ATTR_READ_ONLY ) Code = 'R';
    if( File.Attr & ATTR_HIDDEN    ) Code = 'H';
    if( File.Attr & ATTR_SYSTEM    ) Code = 'S';
    if( File.Attr & ATTR_VOLUME_ID ) Code = 'V';
    if( File.Attr & ATTR_DIRECTORY ) Code = 'D';
    LCD_putc( Code );
  }
  LCD_move(3,0);
  LCD_puts("St. "); LCD_print32( File.Pos.Start, 16 );
  LCD_puts(" Si. "); LCD_print32( File.Size, 10 );
//  while( IRcode() != LG_Enter ) _delay_ms(100); // PAUSE
}


// Change current Directory to "File"
void SDchdir(void)
{
  if( File.Attr & ATTR_DIRECTORY )
  {
    Dir.Pos.Start = File.Pos.Start;
    if( Dir.Pos.Start == 0 ) Dir.Pos.Start = RootClust;
    SDdirStart();
  }
}


// Read File details
void SDfileSelect(void)
{
  uint8_t i;
  // Discard Archive bit
  File.Attr = SDbuffer[Dir.Pos.Byte+11] & 0x1F; 
  // Check if entry is not null (End of Dir)
  if( SDbuffer[Dir.Pos.Byte] == 0 ) File.Attr = ATTR_END;
  // Check if entry is free (deleted)
  if( SDbuffer[Dir.Pos.Byte] == CODE_DELETED ) File.Attr |= ATTR_DELETED;
  // Get short name name
  if( File.Attr != ATTR_LONG_NAME )
  {
    for( i=0; i<11; i++ ) File.Short[i] = SDbuffer[Dir.Pos.Byte+i]; // File Name + Ext
    File.Short[11] = 0;
  }
  // Get start
  if( File.Attr & ATTR_VOLUME_ID ) // Volume or Long Name
    File.Pos.Start = 0;
  else
    File.Pos.Start = (uint32_t)SDbuffer[Dir.Pos.Byte+21]<<24|(uint32_t)SDbuffer[Dir.Pos.Byte+20]<<16|(uint32_t)SDbuffer[Dir.Pos.Byte+27]<<8|(uint32_t)SDbuffer[Dir.Pos.Byte+26];
  // Get size
  if( File.Attr & ATTR_NOFILE ) // Dir, Volume or Long Name
    File.Size  = 0;
  else
    File.Size  = (uint32_t)SDbuffer[Dir.Pos.Byte+31]<<24|(uint32_t)SDbuffer[Dir.Pos.Byte+30]<<16|(uint32_t)SDbuffer[Dir.Pos.Byte+29]<<8|(uint32_t)SDbuffer[Dir.Pos.Byte+28];
  File.Pos.Cluster = File.Pos.Start;
  File.Pos.Block = 0;
  File.Pos.Byte = 0;
  File.Pos.CCount = 0;
  File.LastClust = ( File.Size / ClusterSize / 512UL );
  File.LastBlock = ( File.Size - File.LastClust * ClusterSize * 512UL ) / 512UL;
  File.LastByte  = ( File.Size - File.LastClust * ClusterSize * 512UL - File.LastBlock * 512UL );
}


// Move buffer to entry number "entry" (starting at 0)
uint8_t SDgotoEntry( uint16_t entry )
{
  uint8_t  res;
  uint32_t pos     = (uint32_t)entry * 32UL;
  uint16_t cluster = pos / ClusterSize / 512L;
  GotoCluster( &Dir.Pos, cluster );
  pos              = pos - (uint32_t)cluster * ClusterSize * 512L;
  Dir.Pos.Block    = pos / 512UL;
  Dir.Pos.Byte     = pos - Dir.Pos.Block * 512;
  res = SDloadBuffer( &Dir.Pos );
  return res;
}


// Read Long Name
uint8_t SDreadLong( uint16_t entry )
{
  uint8_t i, offset;
  uint8_t res = SDgotoEntry( entry );
  if( res != SDok ) return SDerror;
  // Get long name
  offset = ( ( SDbuffer[Dir.Pos.Byte] & 0x3F ) - 1 ) * 13;
  for( i=0; i<13; i++ ) File.Name[i+offset] = SDbuffer[Dir.Pos.Byte+lona[i]];
  if( SDbuffer[Dir.Pos.Byte] & 0x40 ) 
  {
    File.Name[13+offset] = 0;
    return SDend;
  }
  else
    return SDok;
}


// Get Attributes of Directory entry number "entry" (starting at 0)
uint8_t SDentryAttr( uint16_t entry )
{
  uint8_t res = SDgotoEntry( entry );
  if( res != SDok ) return ATTR_END;
  // Discard Archive bit
  res = SDbuffer[Dir.Pos.Byte+11] & 0x1F; 
  // Check if entry is not null (End of Dir)
  if( SDbuffer[Dir.Pos.Byte] == 0 ) res = ATTR_END;
  // Check if entry is free (deleted)
  if( SDbuffer[Dir.Pos.Byte] == CODE_DELETED ) res |= ATTR_DELETED;
  // Check if the entry is "."
  if( SDbuffer[Dir.Pos.Byte] == '.' && SDbuffer[Dir.Pos.Byte+1] == ' ' ) res |= ATTR_SYSTEM; 
  return res;
}


// Read Directory entry number "entry" (starting at 0)
uint8_t SDreadEntry( uint16_t entry )
{
  uint8_t res, i, j;
  uint16_t ln;
  File.Name[0] = 0;
  // Try if there is a long name
  if( entry > 0 ) 
  {
    ln = entry-1;
    while( (SDentryAttr(ln) & ATTR_LONG_NAME ) == ATTR_LONG_NAME )
    {
      res = SDreadLong(ln);
      if( res == SDend ) break;
      if(ln>0) ln--; else break;
    }
  }
  // Read Entry data
  res  = SDgotoEntry( entry );
  if( res != SDok ) return res;
  SDfileSelect();
  // Copy Short name in Long name if needed
  if( File.Name[0] == 0 )
  {
    j = 0;
    for( i=0; i<8; i++ ) if( File.Short[i]!=' ' ) File.Name[j++] = File.Short[i];
    if( !( File.Attr & ATTR_DIRECTORY ) )
    {
      File.Name[j++] = '.';
      for( i=8; i<11; i++ ) File.Name[j++] = File.Short[i];
    }
    File.Name[j] = 0;
  }
  return SDok;
}


// Read first entry of current Directory
uint8_t SDdirStart(void)
{
  uint8_t res;
  Dir.Entry = 0;
  if( SDentryAttr(0) & ATTR_SKIP ) 
    res = SDdirNext();
  else
    res = SDreadEntry(0);
  if( res!=SDok ) 
  {
    LCD_line( 1, "No File !" );
    for(;;);
  }
  //else
  //  SDdirPrint();
  return res;
}

// Read next entry of current Directory
uint8_t SDdirNext(void)
{
  uint8_t res;
  uint8_t entry = Dir.Entry + 1;
  while( (res=SDentryAttr(entry)) & ATTR_SKIP ) entry++;  
  if( res != ATTR_END ) Dir.Entry  = entry;
  res = SDreadEntry( Dir.Entry );
  //SDdirPrint();
  return res;
}


// Read previous entry of current Directory
uint8_t SDdirPrev(void)
{
  uint8_t res;
  uint8_t entry;
  if( Dir.Entry>0 ) 
  {
    entry = Dir.Entry - 1;
    while( ((res=SDentryAttr(entry)) & ATTR_SKIP) && (entry>0) ) entry--;
    if( (entry!=0) || !(res & ATTR_SKIP) ) Dir.Entry = entry;
  }
  res = SDreadEntry( Dir.Entry );
  //SDdirPrint();
  return res;
}


// Go to parent directory (..)
uint8_t SDdirParent(void)
{
  uint8_t res;
  if( Dir.Pos.Start != RootClust )
  {
    res = SDreadEntry( 1 );
    SDchdir();
  }
  else
    res = SDdirStart();  
  //SDdirPrint();
  return res;
}


// Go to root directory (/)
uint8_t SDdirRoot(void)
{
  uint8_t res;
  Dir.Pos.Start = RootClust;
  res = SDdirStart();  
  //SDdirPrint();
  return res;
}


// Check if End of File is reached (EOF)
uint8_t SDeof(void)
{
  // Check EOF
  if( File.Pos.CCount > File.LastClust ) return SDerror; 
  if( File.Pos.CCount == File.LastClust && File.Pos.Block > File.LastBlock ) return SDerror;
  if( File.Pos.CCount == File.LastClust && File.Pos.Block == File.LastBlock && File.Pos.Byte >= File.LastByte  ) return SDerror;
  return SDok;
}


// Reset file pointers to Start of file
void SDstartFile(void)
{
  File.Pos.Cluster = File.Pos.Start;
  File.Pos.CCount = 0;
  File.Pos.Block = 0;
  File.Pos.Byte = 0;
}


// Read one byte in current file
uint8_t SDgetc(void)
{
  uint8_t c;
  // Check if it's not a file : Directory or Volume  
  if( ( File.Attr & ATTR_NOFILE ) != 0 ) return 0; 
  // Check EOF
  if( SDeof() != SDok ) return 0; 
  // Change block if required
  if( File.Pos.Byte > 511 ) 
  {
    File.Pos.Block++;
    // Change Cluster if required
    if( File.Pos.Block >= ClusterSize ) 
    {
      File.Pos.Cluster = readFAT( File.Pos.Cluster );
      File.Pos.Block = 0;
      File.Pos.CCount++;
    }
    File.Pos.Byte = 0;
  }
  // Read byte
  SDloadBuffer( &File.Pos );
  c = SDbuffer[File.Pos.Byte];
  File.Pos.Byte++;
  return c;
}



//
// Test : Read file and display it...
//

static char aff[80];

void LCD_add( uint8_t c )
{
  uint8_t i;
  for( i=0; i<79; i++ ) aff[i]=aff[i+1];
  aff[79] = (char)c;
}

void LCD_Show(void)
{
  LCD_line( 0, aff );
  LCD_line( 1, aff+20 );
  LCD_line( 2, aff+40 );
  LCD_line( 3, aff+60 );
}

void SDreadFile(void)
{
  uint8_t c;
  uint32_t i=0, j=0;
  uint32_t IRreceived;
  File.Pos.Block = 0;
  File.Pos.Byte = 0;
  LCD_cls();
  for( j=0; j<79; j++ ) aff[j] = ' '; 
  LCD_add('F'); LCD_add('I'); LCD_add('L'); LCD_add('E'); LCD_add(':'); 
  j = 0;
  for( i = 0; i< File.Size; i++ ) 
  { 
    c = SDgetc();
    if( c < ' ' ) c = ' ';
    LCD_add(c);
    j++;
    if( j > 79 ) 
    { 
      LCD_Show(); 
      j = 0;
      while( ((IRreceived=IRcode())==0) ) _delay_ms(100);
      if( IRreceived==LG_Stop ) return;
    } 
  }
  LCD_add('<'); LCD_add('E'); LCD_add('N'); LCD_add('D'); LCD_add('>'); 
  LCD_Show(); 
  while( IRcode() ==0 ) _delay_ms(100);
  //SDdirPrint();
}