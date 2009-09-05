//      dm2.c - source file for horizontal multi LED display matrix
//              (Sure Electronics 0832 boards)
//
//      these routines support up to 4 boards arranged horizontally.
//      CS0 is the leftmost and is the master.  CS1 is the second board,
//      and is the first slave, and so on.
//
//      there is a total of dm_nboards boards (1 - 4), set by the
//      argument in the call to dm_init().
//
//      each display memory is 4 bit x 64 for our use
//
//      display addresses:  
//      ------------------
//         0  2  4  6  ...  62    <-- top half of display
//         1  3  5  7  ...  63    <-- bottom half of display
//
//        -^--             --^--
//        col1             col32
//
//
//      bit ordering:
//      -------------
//
//      4 bit r/w             8 bit r/w
//      ---------             ---------
//          3   top of display    7
//          2                     6
//          1                     5
//          0                     4
//          3                     3
//          2                     2
//          1                     1
//          0  bottom of display  0
//
//      columns on a single board number from 0 to 31.
//      columns on the group of boards number from 0 to dm_cmax,
//      where dm_cmax = dm_nboards * 32 - 1.
//
//      for ICCAVR all chars are unsigned. in the few places where
//      this is important, unsigned has been explicitly specified.
//
//      function summary
//      ----------------
//      dm_init(#)              initialization
//
//      dm_blankall()           blank all boards
//      dm_blankcols()          blank columns
//      dm_fill()               write pattern to multiple columns
//
//      dm_rcol()               read column
//      dm_wcol()               write column
//      dm_rbytes()             read multiple columns
//      dm_wbytes()             write multiple columns
//
//      dm_lscroll()            scroll display left 1 column
//      dm_rscroll()            scroll display right 1 column
//
//      dm_rbit()               read single bit
//      dm_wbit()               write single bit
//
//      dm_blink()              set blink on/off
//      dm_enable()             set board enable on/off
//      dm_setpwm()             set PWM duty cycle (brightness)
//
//      these functions act on all display boards (1 - 4, set by dm_init).
//      there are low level routines that perform these same functions on
//        the individual boards
//
//
#include <iom16v.h>
#include <AVRdef.h>
#include "dm2.h"

//      local prototypes
char dm_b_fill(char, char, char, char);    // board memory fill
char dm_r_bit(char, char, char);           // read one bit from board
void dm_w_bit(char, char, char, char);     // write one bit to board
void dm_r_scroll(char, char);              // right scroll board
char dm_l_scroll(char, char);              // left scroll board
void dm_multi(char, char);                 // general command to all boards
void dm_cmd(char, char);                   // general command to one board
char dm_r_col(char, char);                 // read column of board
void dm_w_col(char, char, char);           // write column of board
char dm_w_bytes(char, char, char, char *); // write bytes to board
char dm_r_bytes(char, char, char, char *); // read bytes from board
void dm_wakeup(char);                      // select board
char dm_read(char);                        // read 4 or 8 bits (low level)
void dm_write(char, unsigned int);         // send data out (low level)

//      the bit fiddling macros:
#define cs_high()   DM_PORT |= cs_mask
#define cs_low()    DM_PORT &= ~cs_mask
#define data_in()   DM_DDR &= ~DM_DATA; DM_PORT &= ~DM_DATA
#define data_out()  DM_DDR |= DM_DATA

//      local variables
char cs_mask;                      // mask for active board's CS line
char cs_m[] = {DM_CS0, DM_CS1, DM_CS2, DM_CS3};  // all the CS masks

//      global variables (for dm_chars.c functions - do not alter values)
char dm_nboards;                   // number of active boards (1 - 4)
unsigned char dm_cmax;             // rightmost column (31, 63, 95, or 127)


// =================================================================
// ========= the following are the public functions ================
// =================================================================

// ------------------------------------------------------------
//      dm_blink(mode) - set blink on/off
//
//      mode - bit 0 = 0 for off, = 1 for on

void dm_blink(char m) {
  char cmd;

  cmd = (m & 1) ? BLINK_ON : BLINK_OFF;
  dm_multi(1, cmd);
}

// ------------------------------------------------------------
//      dm_enable(mode) - set enable on/off
//
//      mode - bit 0 = 0 for off, = 1 for on

void dm_enable(char m) {
  char cmd;

  cmd = (m & 1) ? SYS_EN : SYS_DIS;
  dm_multi(1, cmd);
}

// ------------------------------------------------------------
//      dm_setpwm(level) - set PWM duty cycle
//
//      level - 0 to 15 (rightmost 4 bits)

void dm_setpwm(char m) {

  m &= 0x0f;                    // keep it legal (0 - 15)
  dm_multi(1, SET_PWM | m);
}

// ------------------------------------------------------------
//      dm_rcol(column) - read a column of display
//
//      column - column to read (0 - dm_cmax)

char dm_rcol(char col) {
  char who;

  if (col > dm_cmax) return 0;  // ignore if out of range
  who = (col >> 5) & 0x03;      // which board it's on
  col &= 0x1f;                  // local column (0 - 31)
  return dm_r_col(who, col);    // go read it
}

// ------------------------------------------------------------
//      dm_wcol(column, value) - write a column to display
//
//      col - column to write (0 - dm_cmax)
//      value - value to write

void dm_wcol(char col, char v) {
  char who;

  if (col <= dm_cmax) {         // only want legal columns
    who = (col >> 5) & 0x03;    // which board it's on
    col &= 0x1f;                // local column (0 - 31)
    dm_w_col(who, col, v);      // go write it
  }
}

// ------------------------------------------------------------
//      dm_rbytes(col, array, n) - read bytes from display
//
//      col - display column to start in (0 - dm_cmax)
//      array[] - where to store bytes
//      n - number of bytes to read (1 - 128)
//
//      returns: number of bytes read

int dm_rbytes(char col, char *p, int n) {
  char who, mx, rtn;
  int r;

  if (col > dm_cmax || n == 0) return 0;
  r = (int) dm_cmax + 1 - col;          // maximum from this column
  if (n > r) n = r;                     // keep it in bounds

  r = 0;                                // count number read
  who = col >> 5;                       // initial board (0 - 3)
  col &= 0x1f;                          // initial column on board

  while (n > 0 && who < dm_nboards) {   // second test is paranoia
    if (n > 32) mx = 32; else mx = n;
    rtn = dm_r_bytes(who, col, mx, p);  // write some
    n -= rtn;                           // that many fewer to go
    r += rtn;                           // that many more read
    p += rtn;                           // advance pointer
    who++;                              // move to next board
    col = 0;                            // starting at its beginning
  }
  return r;
}

// ------------------------------------------------------------
//      dm_wbytes(col, array, n) - write bytes to display
//
//      col - display column to start in (0 - dm_cmax)
//      array[] - bytes to write
//      n - number of bytes to write (1 - 128)
//
//      returns: number of bytes written

int dm_wbytes(char col, char *p, int n) {
  char who, mx, rtn;
  int r;

  if (col > dm_cmax || n == 0) return 0;
  r = (int) dm_cmax + 1 - col;          // maximum from this column
  if (n > r) n = r;                     // keep it in bounds

  r = 0;                                // count number written
  who = col >> 5;                       // initial board (0 - 3)
  col &= 0x1f;                          // initial column on board

  while (n > 0 && who < dm_nboards) {   // second test is paranoia
    if (n > 32) mx = 32; else mx = n;
    rtn = dm_w_bytes(who, col, mx, p);  // write some
    n -= rtn;                           // that many fewer to go
    r += rtn;                           // that many more written
    p += rtn;                           // advance pointer
    who++;                              // move to next board
    col = 0;                            // starting at its beginning
  }
  return r;
}

// ------------------------------------------------------------
//      dm_lscroll(val) - scroll all displays left 1 column
//
//      moves column 1 --> 0, 2 --> 1, ..., dm_cmax --> dm_cmax - 1
//      and inserts val into dm_cmax (rightmost column of all boards)

void dm_lscroll(char v) {
  unsigned char who;

  who = (dm_nboards - 1) & 0x03;  // rightmost board number
  while (who < 4) {
    v = dm_l_scroll(who, v);      // scroll single board
    who--;                        // this will wrap to 255
  }
}

// ------------------------------------------------------------
//      dm_rscroll(val) - scroll all displays right 1 column
//
//      moves column dm_cmax - 1 --> dm_cmax, ..., 0 --> 1
//      and inserts val in column 0

void dm_rscroll(char v) {
  char i, c31;

  for (i = 0; i < dm_nboards; i++) {
    c31 = dm_r_col(i, 31);       // read old column 31
    dm_r_scroll(i, v);
    v = c31;
  }
}

// ------------------------------------------------------------
//      dm_rbit(who, column, bit) - read single bit of display
//
//      column - 0 - dm_cmax
//      bit - 0 to 7 (7 at the top)
//      returns: 0 or 1

char dm_rbit(char col, char bnum) {
  char who;

  if (col > dm_cmax) return 0;        // ignore if out of range
  who = (col >> 5) & 0x03;            // which board it's on
  col &= 0x1f;                        // local column (0 - 31)
  return dm_r_bit(who, col, bnum);    // go read it
}

// ------------------------------------------------------------
//      dm_wbit(column, bit, value) - write single bit to display
//
//      column: 0 - dm_cmax
//      bit:    0 - 7
//      value:  0 or 1 (bit 0 used)

void dm_wbit(char col, char bnum, char v) {
  char who;

  if (col <= dm_cmax) {               // only want legal columns
    who = (col >> 5) & 0x03;          // which board it's on
    col &= 0x1f;                      // local column (0 - 31)
    dm_w_bit(who, col, bnum, v);      // go write it
  }
}

// ------------------------------------------------------------
//      dm_blankall() - blank all boards

void dm_blankall(void) {
  char i;

  for (i = 0; i < dm_nboards; i++)
    dm_b_fill(i, 0, 32, 0);           // blank board's memory
}

// ------------------------------------------------------------
//      dm_blankcols(col, n) - blank some columns
//
//      col - starting column number (0 - dm_cmax)
//        (dm_cmax will be 31, 63, 95, or 127)
//      n - number of columns to blank (1 - 128)
//
//      returns: number of columns blanked

int dm_blankcols(char col, int n) {
  return dm_fill(col, n, 0);
}

// ------------------------------------------------------------
//      dm_fill(col, n, fill) - write pattern to multiple columns
//
//      col - starting column number (0 - dm_cmax)
//        (dm_cmax will be 31, 63, 95, or 127)
//      n - number of columns to write (1 - 128)
//      fill - character to write
//
//      returns: number of columns written

int dm_fill(char col, int n, char f) {
  char who, mx, rtn;
  int r;

  if (col > dm_cmax || n == 0) return 0;
  r = (int) dm_cmax + 1 - col;          // maximum from this column
  if (n > r) n = r;                     // keep it in bounds

  r = 0;
  who = col >> 5;                       // initial board (0 - 3)
  col &= 0x1f;                          // initial column on board

  while (n > 0 && who < dm_nboards) {   // second test is paranoia
    if (n > 32) mx = 32; else mx = n;
    rtn = dm_b_fill(who, col, mx, f);   // fill 'er up
    n -= rtn;                           // that many fewer to go
    r += rtn;                           // that many more written
    who++;                              // move to next board
    col = 0;                            // starting at its beginning
  }
  return r;
}

// ------------------------------------------------------------
//      dm_init(nboards) - initialization
//
//      nboards - number of boards (1 - 4)
//
//      notice that all the I/O lines for the displays have
//      to be on the same port

void dm_init(char n) {
  char p, msk, i;

  if (n < 1 || n > 4) n = 1;
  dm_nboards = n;
  dm_cmax = (n << 5) - 1;               // max column (31, 63, 95, or 127)
  p = DM_RD | DM_WR;                    // the two fixed outputs
  for (i = 0; i < dm_nboards; i++) p |= cs_m[i];  // each board's CS bit

  msk = ~(p | DM_DATA);                 // the "not ours" bits
  DM_DDR &= msk;                        // drop our bits, keep theirs
  DM_PORT &= msk;                       // no pullups on ours
  DM_PORT |= p;                         // turn on pullups briefly
  DM_DDR |= p;                          // and switch to high outputs
  
  // at this point, RD, WR and the CS lines are high (inactive)
  // outputs, and DATA is an input without a pullup
  //
  // the cascade initialization sequence is:
  //   disable master
  //   disable slaves
  //   commons options (master and slave) - default is OK
  //   set master to master
  //   set slaves to slave
  //   sys on for master
  //   sys on for slaves
  //   blank
  //   LEDs on

  dm_multi(1, SYS_DIS);                 // disable master and slaves
  dm_multi(1, COMMONS);                 // all to PMOS 8 (the default)
  dm_cmd(0, SET_MASTER);                // tell the first one it's master
  dm_multi(0, SET_SLAVE);               // tell the others they're slaves
  dm_multi(1, SYS_EN);                  // turn all the buggers on
  dm_blankall();                        // blank everyone's memory
  dm_multi(1, SET_PWM | 7);             // set mid brightness
  dm_multi(1, LED_ON);                  // and away we go
}

// =================================================================
// ========= the following are local low level functions ===========
// =================================================================

// ------------------------------------------------------------
//      dm_b_fill(who, col, n, fill) - fill board's memory
//
//      who  - board to use (0 - 3)
//      col  - starting address (0 - 31)
//      n    - number of columns
//      fill - fill character
//
//      returns: number of bytes written

char dm_b_fill(char who, char col, char n, char fill) {
  unsigned int addr;
  char i;

  if (col > 31 || n == 0) return 0;
  if (n + col > 32) n = 32 - col;

  addr = col << 1;              // they use 4 bit addresses
  dm_wakeup(who);               // make board active
  dm_write(3, 0x05);            // send 101
  dm_write(7, addr);            // nibble address
  for (i = 0; i < n; i++)
    dm_write(8, (unsigned int) fill);   // write 8 bits
  cs_high();                    // end of command
  return n;
}

// ------------------------------------------------------------
//      dm_r_bit(who, column, bit) - read single bit from board
//
//      who - board to use (0 - 3)
//      column - 0 - 31
//      bit - 0 to 7 (7 at the top)
//
//      returns: 0 or 1

char dm_r_bit(char who, char col, char bnum) {
  unsigned int addr;
  char msk, ret;

  addr = col << 1;              // use 4 bit addresses
  if (bnum > 3) {
    msk = 1 << (bnum - 4);      // even address (top)
  } else {
    msk = 1 << bnum;            // odd address (bottom)
    addr++;
  }

  dm_wakeup(who);               // set board active
  dm_write(3, 0x06);            // send 110
  dm_write(7, addr);            // nibble address
  ret = dm_read(0);             // read 4 bits
  cs_high();                    // end of command

  if (ret & msk) ret = 1; else ret = 0;
  return ret;
}

// ------------------------------------------------------------
//      dm_w_bit(who, column, bit, value) - write single bit to board
//
//      who:    board to use (0 - 3)
//      column: 0 - 31
//      bit:    0 - 7
//      value:  0 or 1 (bit 0 used)

void dm_w_bit(char who, char col, char bnum, char v) {
  unsigned int addr, ret, a_msk, o_msk;

  addr = col << 1;              // use 4 bit addresses
  if (bnum > 3) {
    o_msk = 1 << (bnum - 4);    // even address (top)
  } else {
    o_msk = 1 << bnum;          // odd address (bottom)
    addr++;
  }

  a_msk = (~o_msk) & 0x0f;      // the AND mask, in 4 bits
  dm_wakeup(who);               // make board active
  dm_write(3, 0x05);            // send 101
  dm_write(7, addr);            // nibble address
  ret = dm_read(0);             // read 4 bits
  ret &= a_msk;                 // kill our bit
  if (v) ret |= o_msk;          // set our bit, if desired.
  dm_write(4, ret);             // and rewrite 4 bits
  cs_high();                    // end of command
}

// ------------------------------------------------------------
//      dm_r_scroll(who, val) - scroll board right 1 column
//
//      who - board to use (0 - 3)
//      moves column 30 --> 31, 29 --> 30, ..., 0 --> 1
//      and inserts val in column 0

void dm_r_scroll(char who, char v) {
  char h[2], i, t;
  unsigned int r;

  h[0] = v >> 4;                // top 4 bits, new column 0
  h[1] = v & 0x0f;              // bottom 4 bits, new col 0
  dm_wakeup(who);               // make board active
  dm_write(3, 0x5);             // read/mod/write
  dm_write(7, 0);               // address = 0x00

  for (i = 0; i < 64; i++) {
    t = i & 0x01;               // 0 for top, 1 for bottom
    r = h[t];                   // previous value
    h[t] = dm_read(0);          // read 4 bits
    dm_write(4, r);             // write 4 bits
  }
  cs_high();                    // that's all, folks
}

// ------------------------------------------------------------
//      dm_l_scroll(who, val) - scroll board left 1 column
//
//      who - board to use (0 - 3)
//      moves column 1 --> 0, 2 --> 1, ..., 31 --> 30,
//      and inserts val in column 31
//
//      returns: original value in column 0

char dm_l_scroll(char who, char v) {
  char hold[33];

  dm_r_bytes(who, 0, 32, hold);      // read current contents
  hold[32] = v;                      // new value
  dm_w_bytes(who, 0, 32, hold + 1);  // rewrite, offset
  return hold[0];                    // give them column 1
}

// ------------------------------------------------------------
//      dm_multi(who, cmd) - send command to multiple display boards
//
//      who - 0, slaves only; 1, master and slaves
//      cmd - command

void dm_multi(char w, char cmd) {
  char i, bot;

  if (w) {
    bot = 0;                    // include master
  } else {
    if (dm_nboards < 2) return; // no slaves to push around
    bot = 1;                    // exclude master
  }
  for (i = bot; i < dm_nboards; i++) dm_cmd(i, cmd);
}

// ------------------------------------------------------------
//      dm_cmd(who, command) - send command to single board
//
//      who - board to use (0 - 3)
//      command is one of:
//        LED_ON           display LEDs on
//        LED_OFF          display LEDs off
//        SYS_DIS          system disable
//        SYS_EN           system enable
//        BLINK_ON         blink on
//        BLINK_OFF        blink off
//        SET_PWM          set PWM level (see note)
//        SET_MASTER       set board as master
//        SET_SLAVE        set board as slave
//        COMMONS          PMOS 8
//
//      for SET_PWM, OR in a value of 0 - 15 to set PWM level

void dm_cmd(char who, char cmd) {
  unsigned int v;

  dm_wakeup(who);                // which one we're talking to
  v = ((unsigned int) cmd << 1); // commands are 9 bits, bit 0 = 0
  dm_write(3, 0x04);             // send binary 100 as ID
  dm_write(9, v);                // send the 9 bit command
  cs_high();                     // shut it down
}

// ------------------------------------------------------------
//      dm_r_col(who, column) - read column of board
//
//      who - board to use (0 - 3)
//      column - 0 to 31
//      returns the value of the column, bit 0 at top

char dm_r_col(char who, char col) {
  unsigned int addr;
  char ret;

  addr = col << 1;              // they're 4 bit addresses
  dm_wakeup(who);               // make board active
  dm_write(3, 0x06);            // send 110
  dm_write(7, addr);            // nibble address
  ret = dm_read(1);             // read 8 bits
  cs_high();                    // end of command

  return ret;
}

// ------------------------------------------------------------
//      dm_w_col(who, column, value) - write a column of board
//
//      who - board to use (0 - 3)
//      column - 0 to 31
//      value - 8 bits, bit 0 at top

void dm_w_col(char who, char col, char v) {
  unsigned int addr;

  addr = col << 1;                // they're 4 bit addresses
  dm_wakeup(who);                 // make board active
  dm_write(3, 0x05);              // send 101
  dm_write(7, addr);              // nibble address
  dm_write(8, (unsigned int) v);  // write 8 bits
  cs_high();                      // end of command
}

// ------------------------------------------------------------
//      dm_w_bytes(who, col, n, array) - write bytes to a board
//
//      who - board to use (0 - 3)
//      col - display column to start in (0 - 31)
//      n - number of bytes to write (1 - 32)
//      array[] - bytes to write
//
//      returns: number of bytes written

char dm_w_bytes(char who, char col, char n, char *p) {
  unsigned int addr;
  char i;

  if (col > 31 || n == 0) return 0;
  if (n + col > 32) n = 32 - col;

  addr = col << 1;              // they use 4 bit addresses
  dm_wakeup(who);               // make board active
  dm_write(3, 0x05);            // send 101
  dm_write(7, addr);            // nibble address
  for (i = 0; i < n; i++)
    dm_write(8, (unsigned int) *p++);   // write 8 bits
  cs_high();                    // end of command
  return n;
}

// ------------------------------------------------------------
//      dm_r_bytes(who, col, n, array) - read board's memory
//
//      who - board to use (0 - 3)
//      col - starting column (0 - 31)
//      n - number of columns (bytes) to read (1 - 32)
//      array[] - where to put data
//
//      returns: number of bytes read

char dm_r_bytes(char who, char col, char n, char *p) {
  unsigned int addr;
  char i;

  if (col > 31 || n == 0) return 0;
  if (n + col > 32) n = 32 - col;

  addr = col << 1;              // they use 4 bit addresses
  dm_wakeup(who);               // make board active
  dm_write(3, 0x06);            // send 110
  dm_write(7, addr);            // nibble address
  for (i = 0; i < n; i++)
    *p++ = dm_read(1);          // 32 8 bit reads
  cs_high();                    // end of command
  return n;
}

// ------------------------------------------------------------
//      dm_wakeup(who) - set active display board
//
//      who - board to use (0 - 3)

void dm_wakeup(char who) {
  cs_mask = cs_m[who & 0x03];   // mask for our CS bit
  cs_low();                     // take CS low to activate
}

// ------------------------------------------------------------
//      dm_read(w) - read 4 or 8 bits
//
//      w - 0 for 4 bits, 8 otherwise
//
//      note: we're expecting data to (always) be input
//      we read high bit first

char dm_read(char n) {
  char inp, i;

  if(n) n = 8; else n = 4;
  inp = 0;                              // build value here
  for (i = 0; i < n; i++) {
    DM_PORT &= ~DM_RD;                  // bring RD low
    DM_PORT |= DM_RD;                   // bring RD high
    inp <<= 1;                          // make room for our bit
    if (DM_PIN & DM_DATA) inp |= 1;     // if high, OR it in    
  }
  return inp;
}

// ------------------------------------------------------------
//      dm_write(bits, value) - send some bits
//
//      bits - number of bits
//      value - value to send
//
//      we always leave the data line as input
//      high order bit goes out first
//
//      caller needs to call dm_wakeup() to start command, and
//      to call cs_high() to end command.

void dm_write(char nbits, unsigned int v) {
  unsigned int msk;

  msk = 1 << (nbits - 1);
  data_out();                           // we're writing

  while (msk) {
    DM_PORT &= ~DM_WR;                  // bring WR low
    if (msk & v) {
      DM_PORT |= DM_DATA;               // output a 1
    } else {
      DM_PORT &= ~DM_DATA;              // output a 0
    }
    DM_PORT |= DM_WR;                   // bring WR high
    msk >>= 1;                          // next bit
  }
  data_in();                            // leave it as input
}
