#ifndef _SDRead_h_
#define _SDRead_h_

// FAT32 constants
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  0x0F
// Additional ATTR
#define ATTR_DELETED    0x40
#define ATTR_END        0x80
// ATTR Masks
#define ATTR_NOFILE     (ATTR_VOLUME_ID|ATTR_DIRECTORY)
//   Entries o be skipped
#define ATTR_SKIP       (ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID|ATTR_DELETED)

// ode for deleted DIR Entry
#define CODE_DELETED    0xE5

// Command status returned
enum SDstatus
{
  SDok        = 0x00,
  SDnodisk    = 0x01,
  SDtimeout   = 0x02,
  SDbadformat = 0x03,
  SDend       = 0x04,
  SDerror     = 0xFF
};

// File/Dir Structure
typedef struct PosStruct
{
  uint32_t  Start;     // First Cluster
  uint32_t  Cluster;   // Actual Cluster
  uint16_t  Block;     // Actual Block
  uint16_t  Byte;      // Actual Position in Actual Block
  uint16_t  CCount;    // Count of previous Clusters
} PosStruct;

typedef struct FileStruct
{
  char      Short[12]; // Short File Name 8.3
  char      Name[100]; // Long File Name
  uint8_t   Attr;      // File Attributes
  uint32_t  Size;      // File Size
  PosStruct Pos;       // File actual Position
  uint16_t  LastClust; // Last Cluster number
  uint16_t  LastBlock; // Last Block in Last Cluster
  uint16_t  LastByte;  // Last Byte in Last Block in Last Cluster
} FileStruct;

typedef struct DirStruct
{
  uint16_t  Entry;     // Actual Dir Entry
  PosStruct Pos;       // Dir actual Position Cluster
} DirStruct;

// File and Dir data
extern DirStruct  Dir;
extern FileStruct File;
extern DirStruct Memory[];

// Commands
uint8_t SDinit(void);
uint8_t SDboot(void);

uint8_t SDdirStart(void);
uint8_t SDdirNext(void);
uint8_t SDdirPrev(void);
uint8_t SDdirParent(void);
uint8_t SDdirRoot(void);

void    SDchdir(void);
void    SDfileSelect(void);
uint8_t SDreadEntry( uint16_t entry );

void    SDstartFile(void);
uint8_t SDgetc(void);
uint8_t SDdeof(void);

void SDreadFile(void);

#endif