/*--------------------------------------------------------------------
  cMorseWords5.c - Send sequence of words selected randomly from a
                   list of 500 words or send groups of five randomly
                   selected characters for Morse code practice.

  Operation:  On startup, goes to speed selection screen.  Speed is
              selected with UP and DOWN pushbuttons, between MAX_WPM
              and MIN_WPM.  
              
              When MENU/SELECT button is pushed again the user may
              select to either receive words or random character
              groups using the UP and DOWN buttons.  
              
              If words are selected, the next push of MENU/SELECT
              allows the user to choose the size of the most common 
              word list used, 100, 200, 300, 400, or 500.

              If random groups are selected, the user has the choice
              of either letters or numbers only, letters and numbers,
              or all characters.

              When MENU/SELECT is pushed again, the program reseeds
              the random number generator with a running timer value
              and begins to send.

              While sending is occuring, the DOWN and UP buttons
              act as PAUSE and CONTINUE buttons, respectively.  
              PAUSE (DOWN) freezes the display so the user can check
              a word or group that has just been sent.  It must be
              held down until the current word or group has been
              completely sent.  The CONTINUE (UP) button unfreezes
              the display and sending continues.

              The MENU/SELECT button may be pushed during sending to
              get the menu screens again.  Note that this button
              also must be held down until the current word or
              group has been completely sent.

  Hardware:  ATMega8 using internal RC oscillator at 1 MHz

    Pushbutton inputs:
      PD5 to UP/CONTINUE
      PD6 to DOWN/PAUSE
      PD7 to MENU/SELECT

    PORTC connected to HD4478 LCD display using 4/6 wire 
    connections and simplified timing and pin assignments:
      PC0 to LCD pin 11-D4
      PC1 to LCD pin 12-D5
      PC2 to LCD pin 13-D6
      PC3 to LCD pin 14-D7
      PC4 to LCD pin 6-E
      PC5 to LCD pin 4-R/S

      Note:  LCD pin 5-R/W is held at ground.

    PB1/OC1A is sidetone output, a square wave at 680 Hz.
    PB0 is keyed line output for an external code practice oscillator.

  Notes:  Uses TIMER1 to generate the 680 Hz sidetone and provide a
          new random seed when the program is (re)started.

          This version was written to use only basic delay loops and
          assumes a 1 MHz system clock.  Libc basic delay loop
          function _delay_loop_2(unsigned int count) uses 4 system
          clock cycles per count (plus some set up overhead), so here
          is 4 us per count here.  A delay of 1 ms gives a count of
          250 (ignoring overhead).

          The Libc long integer random number function is used to
          avoid floating point operations used in previous versions.

          Changing to basic delay loops and eliminating floating
          point math significantly reduces code size.

  Andrew Palm
  2008.12.26
--------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/atomic.h>

// Helpful general defines
#define clear_bit( reg, bitnum )  ((reg) &= ~(1<<(bitnum)))
#define set_bit( reg, bitnum )    ((reg) |= (1<<(bitnum)))
#define test_bit( reg, bitnum )   ((reg) & (1<<(bitnum)))
#define toggle_bit( reg, bitnum ) ((reg) ^= (1<<(bitnum)))
// For retrieval of single byte in string stored in program memory
#define pgm_read_string_byte(s, i) \
             pgm_read_byte((PGM_P) (pgm_read_word(&(s))+i))

// LCD output
#define LCD_PORT PORTC    // Set port for LCD
#define LCD_DDR DDRC
#define E_BIT_NUM  4      // LCD Control Pin numbers
#define RS_BIT_NUM 5

// Keyed line output and control
#define OUTPORT PORTB
#define OUTDDR  DDRB
#define OUTBIT  0         // Keyed output
#define KEY_ON  set_bit(OUTPORT, OUTBIT)
#define KEY_OFF clear_bit(OUTPORT, OUTBIT)

// Pushbutton inputs and speed limits
#define PBPIN  PIND
#define PBPORT PORTD
#define PBDDR  DDRD
#define MENU_SELECT 7
#define DOWN        6 
#define UP          5
#define MAX_WPM 35  // Maximum WPM
#define MIN_WPM 10  // Minimum WPM

// Display buffer
#define BUFFER_SIZE 32  // Circular buffer can hold 31 characters
#define mod_buff_size & 0x1F  // To avoid conflicts with % op

// Word parameters
#define MAX_WORD_LEN 8
#define MAX_NUM_WORDS 500
#define MIN_NUM_WORDS 100

// Character parameter
#define GROUP_LEN 5
#define NUM_CHARS       40  
#define NUM_LETTERS     26
#define NUM_LET_AND_NUM 36
#define NUM_NUMBERS     10
#define MAX_CHAR_STATE 3
#define MIN_CHAR_STATE 0

// Sidetone output and control
#define SIDETONE_DDR DDRB
#define SIDETONE_BIT 1
#define SIDETONE_ON  set_bit(SIDETONE_DDR, SIDETONE_BIT)
#define SIDETONE_OFF clear_bit(SIDETONE_DDR, SIDETONE_BIT)
#define SIDETONE_FREQ 680
// Microseconds per half period of sidetone square wave
#define HALF_PERIOD_uS ((1000000.0 / ((float) SIDETONE_FREQ)) / 2.0)
// Microseconds per clock tick assuming timer same as system clock
#define uS_PER_CLOCK_TICK (1000000.0 / ((float) F_CPU))

// Clock ticks per half period
const unsigned int TICKS_PER_HP = (HALF_PERIOD_uS / uS_PER_CLOCK_TICK) + 0.5;

// Delay loop count definitions for _delay_loop_2() for 1 MHz system clock
// and 4 clock ticks per loop iteration
#define uS_PER_LOOP_ITER 4
#define CNT_200uS (200/uS_PER_LOOP_ITER)
#define CNT_1MS   (1000/uS_PER_LOOP_ITER)
#define CNT_5MS   (5000/uS_PER_LOOP_ITER)
#define CNT_20MS  (20000/uS_PER_LOOP_ITER)
#define CNT_x_WPM 300000    // 1 dit = 1200/wpm milliseconds
                            //   = CNT_1MS*1200/wpm loop iterations

const unsigned char msg1[] = "WPM: ";
const unsigned char msg2[] = "SEND WORDS ";
const unsigned char msg3[] = "SEND GROUPS";
const unsigned char msg4[] = "00 MOST COMMON";
const unsigned char msg5[] = "ALL CHARACTERS ";
const unsigned char msg6[] = "LETTERS ONLY   ";
const unsigned char msg7[] = "NUMBERS ONLY   ";
const unsigned char msg8[] = "LETTERS NUMBERS";

// Lookup table for Morse code.  Index is calculated from LCD char
// value.  Reading from least to most sig. bit:
//
//   Filler zeros (not sent)
//   Start bit (not sent)
//   Bits to be sent, 1 = dit, 0 = dah (reverse order)

const char CODE_TABLE[54] = { 0b00000001,  // Flag to send blank
                              0b10000110,  // '
                              0b10010100,  // (
                              0b01001010,  // )
                              0b00000000,  // * - not used
                              0b10101100,  // + or AR
                              0b00110010,  // comma
                              0b01111010,  // minus or long break
                              0b01010110,  // period
                              0b10110100,  // slash
                              0b00000100,  // 0
                              0b00001100,  // 1
                              0b00011100,  // 2
                              0b00111100,  // 3
                              0b01111100,  // 4
                              0b11111100,  // 5
                              0b11110100,  // 6
                              0b11100100,  // 7
                              0b11000100,  // 8
                              0b10000100,  // 9
                              0b11100010,  // colon
                              0b10101010,  // semicolon
                              0b00000000,  // less than - not used
                              0b01110100,  // equals or short break BT
                              0b01011110,  // grtr than = SK
                              0b11001110,  // question mark
                              0b10100110,  // @ symbol
                              0b01100000,  // A
                              0b11101000,  // B
                              0b10101000,  // C
                              0b11010000,  // D
                              0b11000000,  // E
                              0b10111000,  // F
                              0b10010000,  // G
                              0b11111000,  // H
                              0b11100000,  // I
                              0b00011000,  // J
                              0b01010000,  // K
                              0b11011000,  // L
                              0b00100000,  // M
                              0b10100000,  // N
                              0b00010000,  // O
                              0b10011000,  // P
                              0b01001000,  // Q
                              0b10110000,  // R
                              0b11110000,  // S
                              0b01000000,  // T
                              0b01110000,  // U
                              0b01111000,  // V
                              0b00110000,  // W
                              0b01101000,  // X
                              0b00101000,  // Y
                              0b11001000,  // Z
                              0b00000000}; // Null - send nothing

// Words to be sent randomly stored in program memory
char word000[] PROGMEM = "THE";
char word001[] PROGMEM = "OF";
char word002[] PROGMEM = "TO";
char word003[] PROGMEM = "AND";
char word004[] PROGMEM = "A";
char word005[] PROGMEM = "IN";
char word006[] PROGMEM = "IS";
char word007[] PROGMEM = "IT";
char word008[] PROGMEM = "YOU";
char word009[] PROGMEM = "THAT";
char word010[] PROGMEM = "HE";
char word011[] PROGMEM = "WAS";
char word012[] PROGMEM = "FOR";
char word013[] PROGMEM = "ON";
char word014[] PROGMEM = "ARE";
char word015[] PROGMEM = "WITH";
char word016[] PROGMEM = "AS";
char word017[] PROGMEM = "I";
char word018[] PROGMEM = "HIS";
char word019[] PROGMEM = "THEY";
char word020[] PROGMEM = "BE";
char word021[] PROGMEM = "AT";
char word022[] PROGMEM = "ONE";
char word023[] PROGMEM = "HAVE";
char word024[] PROGMEM = "THIS";
char word025[] PROGMEM = "FROM";
char word026[] PROGMEM = "OR";
char word027[] PROGMEM = "HAD";
char word028[] PROGMEM = "BY";
char word029[] PROGMEM = "HOT";
char word030[] PROGMEM = "BUT";
char word031[] PROGMEM = "SOME";
char word032[] PROGMEM = "WHAT";
char word033[] PROGMEM = "THERE";
char word034[] PROGMEM = "WE";
char word035[] PROGMEM = "CAN";
char word036[] PROGMEM = "OUT";
char word037[] PROGMEM = "OTHER";
char word038[] PROGMEM = "WERE";
char word039[] PROGMEM = "ALL";
char word040[] PROGMEM = "YOUR";
char word041[] PROGMEM = "WHEN";
char word042[] PROGMEM = "UP";
char word043[] PROGMEM = "USE";
char word044[] PROGMEM = "WORD";
char word045[] PROGMEM = "HOW";
char word046[] PROGMEM = "SAID";
char word047[] PROGMEM = "AN";
char word048[] PROGMEM = "EACH";
char word049[] PROGMEM = "SHE";
char word050[] PROGMEM = "WHICH";
char word051[] PROGMEM = "DO";
char word052[] PROGMEM = "THEIR";
char word053[] PROGMEM = "TIME";
char word054[] PROGMEM = "IF";
char word055[] PROGMEM = "WILL";
char word056[] PROGMEM = "WAY";
char word057[] PROGMEM = "ABOUT";
char word058[] PROGMEM = "MANY";
char word059[] PROGMEM = "THEN";
char word060[] PROGMEM = "THEM";
char word061[] PROGMEM = "WOULD";
char word062[] PROGMEM = "WRITE";
char word063[] PROGMEM = "LIKE";
char word064[] PROGMEM = "SO";
char word065[] PROGMEM = "THESE";
char word066[] PROGMEM = "HER";
char word067[] PROGMEM = "LONG";
char word068[] PROGMEM = "MAKE";
char word069[] PROGMEM = "THING";
char word070[] PROGMEM = "SEE";
char word071[] PROGMEM = "HIM";
char word072[] PROGMEM = "TWO";
char word073[] PROGMEM = "HAS";
char word074[] PROGMEM = "LOOK";
char word075[] PROGMEM = "MORE";
char word076[] PROGMEM = "DAY";
char word077[] PROGMEM = "COULD";
char word078[] PROGMEM = "GO";
char word079[] PROGMEM = "COME";
char word080[] PROGMEM = "DID";
char word081[] PROGMEM = "MY";
char word082[] PROGMEM = "SOUND";
char word083[] PROGMEM = "NO";
char word084[] PROGMEM = "MOST";
char word085[] PROGMEM = "NUMBER";
char word086[] PROGMEM = "WHO";
char word087[] PROGMEM = "OVER";
char word088[] PROGMEM = "KNOW";
char word089[] PROGMEM = "WATER";
char word090[] PROGMEM = "THAN";
char word091[] PROGMEM = "CALL";
char word092[] PROGMEM = "FIRST";
char word093[] PROGMEM = "PEOPLE";
char word094[] PROGMEM = "MAY";
char word095[] PROGMEM = "DOWN";
char word096[] PROGMEM = "SIDE";
char word097[] PROGMEM = "BEEN";
char word098[] PROGMEM = "NOW";
char word099[] PROGMEM = "FIND";
char word100[] PROGMEM = "ANY";
char word101[] PROGMEM = "NEW";
char word102[] PROGMEM = "WORK";
char word103[] PROGMEM = "PART";
char word104[] PROGMEM = "TAKE";
char word105[] PROGMEM = "GET";
char word106[] PROGMEM = "PLACE";
char word107[] PROGMEM = "MADE";
char word108[] PROGMEM = "LIVE";
char word109[] PROGMEM = "WHERE";
char word110[] PROGMEM = "AFTER";
char word111[] PROGMEM = "BACK";
char word112[] PROGMEM = "LITTLE";
char word113[] PROGMEM = "ONLY";
char word114[] PROGMEM = "ROUND";
char word115[] PROGMEM = "MAN";
char word116[] PROGMEM = "YEAR";
char word117[] PROGMEM = "CAME";
char word118[] PROGMEM = "SHOW";
char word119[] PROGMEM = "EVERY";
char word120[] PROGMEM = "GOOD";
char word121[] PROGMEM = "ME";
char word122[] PROGMEM = "GIVE";
char word123[] PROGMEM = "OUR";
char word124[] PROGMEM = "UNDER";
char word125[] PROGMEM = "OPEN";
char word126[] PROGMEM = "SEEM";
char word127[] PROGMEM = "TOGETHER";
char word128[] PROGMEM = "NEXT";
char word129[] PROGMEM = "WHITE";
char word130[] PROGMEM = "CHILDREN";
char word131[] PROGMEM = "BEGIN";
char word132[] PROGMEM = "GOT";
char word133[] PROGMEM = "WALK";
char word134[] PROGMEM = "EXAMPLE";
char word135[] PROGMEM = "EASE";
char word136[] PROGMEM = "PAPER";
char word137[] PROGMEM = "OFTEN";
char word138[] PROGMEM = "ALWAYS";
char word139[] PROGMEM = "MUSIC";
char word140[] PROGMEM = "THOSE";
char word141[] PROGMEM = "BOTH";
char word142[] PROGMEM = "MARK";
char word143[] PROGMEM = "BOOK";
char word144[] PROGMEM = "LETTER";
char word145[] PROGMEM = "UNTIL";
char word146[] PROGMEM = "MILE";
char word147[] PROGMEM = "RIVER";
char word148[] PROGMEM = "CAR";
char word149[] PROGMEM = "FEET";
char word150[] PROGMEM = "CARE";
char word151[] PROGMEM = "SECOND";
char word152[] PROGMEM = "GROUP";
char word153[] PROGMEM = "CARRY";
char word154[] PROGMEM = "TOOK";
char word155[] PROGMEM = "RAIN";
char word156[] PROGMEM = "EAT";
char word157[] PROGMEM = "ROOM";
char word158[] PROGMEM = "FRIEND";
char word159[] PROGMEM = "BEGAN";
char word160[] PROGMEM = "IDEA";
char word161[] PROGMEM = "FISH";
char word162[] PROGMEM = "MOUNTAIN";
char word163[] PROGMEM = "NORTH";
char word164[] PROGMEM = "ONCE";
char word165[] PROGMEM = "BASE";
char word166[] PROGMEM = "HEAR";
char word167[] PROGMEM = "HORSE";
char word168[] PROGMEM = "CUT";
char word169[] PROGMEM = "SURE";
char word170[] PROGMEM = "WATCH";
char word171[] PROGMEM = "COLOR";
char word172[] PROGMEM = "FACE";
char word173[] PROGMEM = "WOOD";
char word174[] PROGMEM = "MAIN";
char word175[] PROGMEM = "ENOUGH";
char word176[] PROGMEM = "PLAIN";
char word177[] PROGMEM = "GIRL";
char word178[] PROGMEM = "USUAL";
char word179[] PROGMEM = "YOUNG";
char word180[] PROGMEM = "READY";
char word181[] PROGMEM = "ABOVE";
char word182[] PROGMEM = "EVER";
char word183[] PROGMEM = "RED";
char word184[] PROGMEM = "LIST";
char word185[] PROGMEM = "THOUGH";
char word186[] PROGMEM = "FEEL";
char word187[] PROGMEM = "TALK";
char word188[] PROGMEM = "BIRD";
char word189[] PROGMEM = "SOON";
char word190[] PROGMEM = "BODY";
char word191[] PROGMEM = "DOG";
char word192[] PROGMEM = "FAMILY";
char word193[] PROGMEM = "DIRECT";
char word194[] PROGMEM = "POSE";
char word195[] PROGMEM = "LEAVE";
char word196[] PROGMEM = "SONG";
char word197[] PROGMEM = "MEASURE";
char word198[] PROGMEM = "STATE";
char word199[] PROGMEM = "PRODUCT";
char word200[] PROGMEM = "BLACK";
char word201[] PROGMEM = "SHORT";
char word202[] PROGMEM = "NUMERAL";
char word203[] PROGMEM = "CLASS";
char word204[] PROGMEM = "WIND";
char word205[] PROGMEM = "QUESTION";
char word206[] PROGMEM = "HAPPEN";
char word207[] PROGMEM = "COMPLETE";
char word208[] PROGMEM = "SHIP";
char word209[] PROGMEM = "AREA";
char word210[] PROGMEM = "HALF";
char word211[] PROGMEM = "ROCK";
char word212[] PROGMEM = "ORDER";
char word213[] PROGMEM = "FIRE";
char word214[] PROGMEM = "SOUTH";
char word215[] PROGMEM = "PROBLEM";
char word216[] PROGMEM = "PIECE";
char word217[] PROGMEM = "TOLD";
char word218[] PROGMEM = "KNEW";
char word219[] PROGMEM = "PASS";
char word220[] PROGMEM = "FARM";
char word221[] PROGMEM = "TOP";
char word222[] PROGMEM = "WHOLE";
char word223[] PROGMEM = "KING";
char word224[] PROGMEM = "SIZE";
char word225[] PROGMEM = "HEARD";
char word226[] PROGMEM = "BEST";
char word227[] PROGMEM = "HOUR";
char word228[] PROGMEM = "BETTER";
char word229[] PROGMEM = "TRUE";
char word230[] PROGMEM = "DURING";
char word231[] PROGMEM = "HUNDRED";
char word232[] PROGMEM = "AM";
char word233[] PROGMEM = "REMEMBER";
char word234[] PROGMEM = "STEP";
char word235[] PROGMEM = "EARLY";
char word236[] PROGMEM = "HOLD";
char word237[] PROGMEM = "WEST";
char word238[] PROGMEM = "GROUND";
char word239[] PROGMEM = "INTEREST";
char word240[] PROGMEM = "REACH";
char word241[] PROGMEM = "FAST";
char word242[] PROGMEM = "FIVE";
char word243[] PROGMEM = "SING";
char word244[] PROGMEM = "LISTEN";
char word245[] PROGMEM = "SIX";
char word246[] PROGMEM = "TABLE";
char word247[] PROGMEM = "TRAVEL";
char word248[] PROGMEM = "LESS";
char word249[] PROGMEM = "MORNING";
char word250[] PROGMEM = "NAME";
char word251[] PROGMEM = "VERY";
char word252[] PROGMEM = "THROUGH";
char word253[] PROGMEM = "JUST";
char word254[] PROGMEM = "FORM";
char word255[] PROGMEM = "MUCH";
char word256[] PROGMEM = "GREAT";
char word257[] PROGMEM = "THINK";
char word258[] PROGMEM = "SAY";
char word259[] PROGMEM = "HELP";
char word260[] PROGMEM = "LOW";
char word261[] PROGMEM = "LINE";
char word262[] PROGMEM = "BEFORE";
char word263[] PROGMEM = "TURN";
char word264[] PROGMEM = "CAUSE";
char word265[] PROGMEM = "SAME";
char word266[] PROGMEM = "MEAN";
char word267[] PROGMEM = "DIFFER";
char word268[] PROGMEM = "MOVE";
char word269[] PROGMEM = "RIGHT";
char word270[] PROGMEM = "BOY";
char word271[] PROGMEM = "OLD";
char word272[] PROGMEM = "TOO";
char word273[] PROGMEM = "DOES";
char word274[] PROGMEM = "TELL";
char word275[] PROGMEM = "SENTENCE";
char word276[] PROGMEM = "SET";
char word277[] PROGMEM = "THREE";
char word278[] PROGMEM = "WANT";
char word279[] PROGMEM = "AIR";
char word280[] PROGMEM = "WELL";
char word281[] PROGMEM = "ALSO";
char word282[] PROGMEM = "PLAY";
char word283[] PROGMEM = "SMALL";
char word284[] PROGMEM = "END";
char word285[] PROGMEM = "PUT";
char word286[] PROGMEM = "HOME";
char word287[] PROGMEM = "READ";
char word288[] PROGMEM = "HAND";
char word289[] PROGMEM = "PORT";
char word290[] PROGMEM = "LARGE";
char word291[] PROGMEM = "SPELL";
char word292[] PROGMEM = "ADD";
char word293[] PROGMEM = "EVEN";
char word294[] PROGMEM = "LAND";
char word295[] PROGMEM = "HERE";
char word296[] PROGMEM = "MUST";
char word297[] PROGMEM = "BIG";
char word298[] PROGMEM = "HIGH";
char word299[] PROGMEM = "SUCH";
char word300[] PROGMEM = "FOLLOW";
char word301[] PROGMEM = "ACT";
char word302[] PROGMEM = "WHY";
char word303[] PROGMEM = "ASK";
char word304[] PROGMEM = "MEN";
char word305[] PROGMEM = "CHANGE";
char word306[] PROGMEM = "WENT";
char word307[] PROGMEM = "LIGHT";
char word308[] PROGMEM = "KIND";
char word309[] PROGMEM = "OFF";
char word310[] PROGMEM = "NEED";
char word311[] PROGMEM = "HOUSE";
char word312[] PROGMEM = "PICTURE";
char word313[] PROGMEM = "TRY";
char word314[] PROGMEM = "US";
char word315[] PROGMEM = "AGAIN";
char word316[] PROGMEM = "ANIMAL";
char word317[] PROGMEM = "POINT";
char word318[] PROGMEM = "MOTHER";
char word319[] PROGMEM = "WORLD";
char word320[] PROGMEM = "NEAR";
char word321[] PROGMEM = "BUILD";
char word322[] PROGMEM = "SELF";
char word323[] PROGMEM = "EARTH";
char word324[] PROGMEM = "FATHER";
char word325[] PROGMEM = "HEAD";
char word326[] PROGMEM = "STAND";
char word327[] PROGMEM = "OWN";
char word328[] PROGMEM = "PAGE";
char word329[] PROGMEM = "SHOULD";
char word330[] PROGMEM = "COUNTRY";
char word331[] PROGMEM = "FOUND";
char word332[] PROGMEM = "ANSWER";
char word333[] PROGMEM = "SCHOOL";
char word334[] PROGMEM = "GROW";
char word335[] PROGMEM = "STUDY";
char word336[] PROGMEM = "STILL";
char word337[] PROGMEM = "LEARN";
char word338[] PROGMEM = "PLANT";
char word339[] PROGMEM = "COVER";
char word340[] PROGMEM = "FOOD";
char word341[] PROGMEM = "SUN";
char word342[] PROGMEM = "FOUR";
char word343[] PROGMEM = "THOUGHT";
char word344[] PROGMEM = "LET";
char word345[] PROGMEM = "KEEP";
char word346[] PROGMEM = "EYE";
char word347[] PROGMEM = "NEVER";
char word348[] PROGMEM = "LAST";
char word349[] PROGMEM = "DOOR";
char word350[] PROGMEM = "BETWEEN";
char word351[] PROGMEM = "CITY";
char word352[] PROGMEM = "TREE";
char word353[] PROGMEM = "CROSS";
char word354[] PROGMEM = "SINCE";
char word355[] PROGMEM = "HARD";
char word356[] PROGMEM = "START";
char word357[] PROGMEM = "MIGHT";
char word358[] PROGMEM = "STORY";
char word359[] PROGMEM = "SAW";
char word360[] PROGMEM = "FAR";
char word361[] PROGMEM = "SEA";
char word362[] PROGMEM = "DRAW";
char word363[] PROGMEM = "LEFT";
char word364[] PROGMEM = "LATE";
char word365[] PROGMEM = "RUN";
char word366[] PROGMEM = "DON'T";
char word367[] PROGMEM = "WHILE";
char word368[] PROGMEM = "PRESS";
char word369[] PROGMEM = "CLOSE";
char word370[] PROGMEM = "NIGHT";
char word371[] PROGMEM = "REAL";
char word372[] PROGMEM = "LIFE";
char word373[] PROGMEM = "FEW";
char word374[] PROGMEM = "STOP";
char word375[] PROGMEM = "TEN";
char word376[] PROGMEM = "SIMPLE";
char word377[] PROGMEM = "SEVERAL";
char word378[] PROGMEM = "VOWEL";
char word379[] PROGMEM = "TOWARD";
char word380[] PROGMEM = "WAR";
char word381[] PROGMEM = "LAY";
char word382[] PROGMEM = "AGAINST";
char word383[] PROGMEM = "PATTERN";
char word384[] PROGMEM = "SLOW";
char word385[] PROGMEM = "CENTER";
char word386[] PROGMEM = "LOVE";
char word387[] PROGMEM = "PERSON";
char word388[] PROGMEM = "MONEY";
char word389[] PROGMEM = "SERVE";
char word390[] PROGMEM = "APPEAR";
char word391[] PROGMEM = "ROAD";
char word392[] PROGMEM = "MAP";
char word393[] PROGMEM = "SCIENCE";
char word394[] PROGMEM = "RULE";
char word395[] PROGMEM = "GOVERN";
char word396[] PROGMEM = "PULL";
char word397[] PROGMEM = "COLD";
char word398[] PROGMEM = "NOTICE";
char word399[] PROGMEM = "VOICE";
char word400[] PROGMEM = "FALL";
char word401[] PROGMEM = "POWER";
char word402[] PROGMEM = "TOWN";
char word403[] PROGMEM = "FINE";
char word404[] PROGMEM = "CERTAIN";
char word405[] PROGMEM = "FLY";
char word406[] PROGMEM = "UNIT";
char word407[] PROGMEM = "LEAD";
char word408[] PROGMEM = "CRY";
char word409[] PROGMEM = "DARK";
char word410[] PROGMEM = "MACHINE";
char word411[] PROGMEM = "NOTE";
char word412[] PROGMEM = "WAIT";
char word413[] PROGMEM = "PLAN";
char word414[] PROGMEM = "FIGURE";
char word415[] PROGMEM = "STAR";
char word416[] PROGMEM = "BOX";
char word417[] PROGMEM = "NOUN";
char word418[] PROGMEM = "FIELD";
char word419[] PROGMEM = "REST";
char word420[] PROGMEM = "CORRECT";
char word421[] PROGMEM = "ABLE";
char word422[] PROGMEM = "POUND";
char word423[] PROGMEM = "DONE";
char word424[] PROGMEM = "BEAUTY";
char word425[] PROGMEM = "DRIVE";
char word426[] PROGMEM = "STOOD";
char word427[] PROGMEM = "CONTAIN";
char word428[] PROGMEM = "FRONT";
char word429[] PROGMEM = "TEACH";
char word430[] PROGMEM = "WEEK";
char word431[] PROGMEM = "FINAL";
char word432[] PROGMEM = "GAVE";
char word433[] PROGMEM = "GREEN";
char word434[] PROGMEM = "OH";
char word435[] PROGMEM = "QUICK";
char word436[] PROGMEM = "DEVELOP";
char word437[] PROGMEM = "SLEEP";
char word438[] PROGMEM = "WARM";
char word439[] PROGMEM = "FREE";
char word440[] PROGMEM = "MINUTE";
char word441[] PROGMEM = "STRONG";
char word442[] PROGMEM = "SPECIAL";
char word443[] PROGMEM = "MIND";
char word444[] PROGMEM = "BEHIND";
char word445[] PROGMEM = "CLEAR";
char word446[] PROGMEM = "TAIL";
char word447[] PROGMEM = "PRODUCE";
char word448[] PROGMEM = "FACT";
char word449[] PROGMEM = "STREET";
char word450[] PROGMEM = "INCH";
char word451[] PROGMEM = "LOT";
char word452[] PROGMEM = "NOTHING";
char word453[] PROGMEM = "COURSE";
char word454[] PROGMEM = "STAY";
char word455[] PROGMEM = "WHEEL";
char word456[] PROGMEM = "FULL";
char word457[] PROGMEM = "FORCE";
char word458[] PROGMEM = "BLUE";
char word459[] PROGMEM = "OBJECT";
char word460[] PROGMEM = "DECIDE";
char word461[] PROGMEM = "SURFACE";
char word462[] PROGMEM = "DEEP";
char word463[] PROGMEM = "MOON";
char word464[] PROGMEM = "ISLAND";
char word465[] PROGMEM = "FOOT";
char word466[] PROGMEM = "YET";
char word467[] PROGMEM = "BUSY";
char word468[] PROGMEM = "TEST";
char word469[] PROGMEM = "RECORD";
char word470[] PROGMEM = "BOAT";
char word471[] PROGMEM = "COMMON";
char word472[] PROGMEM = "GOLD";
char word473[] PROGMEM = "POSSIBLE";
char word474[] PROGMEM = "PLANE";
char word475[] PROGMEM = "AGE";
char word476[] PROGMEM = "DRY";
char word477[] PROGMEM = "WONDER";
char word478[] PROGMEM = "LAUGH";
char word479[] PROGMEM = "THOUSAND";
char word480[] PROGMEM = "AGO";
char word481[] PROGMEM = "RAN";
char word482[] PROGMEM = "CHECK";
char word483[] PROGMEM = "GAME";
char word484[] PROGMEM = "SHAPE";
char word485[] PROGMEM = "YES";
char word486[] PROGMEM = "HOT";
char word487[] PROGMEM = "MISS";
char word488[] PROGMEM = "BROUGHT";
char word489[] PROGMEM = "HEAT";
char word490[] PROGMEM = "SNOW";
char word491[] PROGMEM = "BED";
char word492[] PROGMEM = "BRING";
char word493[] PROGMEM = "SIT";
char word494[] PROGMEM = "PERHAPS";
char word495[] PROGMEM = "FILL";
char word496[] PROGMEM = "EAST";
char word497[] PROGMEM = "WEIGHT";
char word498[] PROGMEM = "LANGUAGE";
char word499[] PROGMEM = "AMONG";

PGM_P word_ptr[500] PROGMEM = {
  word000, word001, word002, word003, word004, 
  word005, word006, word007, word008, word009, 
  word010, word011, word012, word013, word014, 
  word015, word016, word017, word018, word019,
  word020, word021, word022, word023, word024,
  word025, word026, word027, word028, word029,
  word030, word031, word032, word033, word034,
  word035, word036, word037, word038, word039,
  word040, word041, word042, word043, word044,
  word045, word046, word047, word048, word049,
  word050, word051, word052, word053, word054,
  word055, word056, word057, word058, word059,
  word060, word061, word062, word063, word064,
  word065, word066, word067, word068, word069,
  word070, word071, word072, word073, word074,
  word075, word076, word077, word078, word079,
  word080, word081, word082, word083, word084,
  word085, word086, word087, word088, word089,
  word090, word091, word092, word093, word094,
  word095, word096, word097, word098, word099,
  word100, word101, word102, word103, word104,
  word105, word106, word107, word108, word109,
  word110, word111, word112, word113, word114,
  word115, word116, word117, word118, word119,
  word120, word121, word122, word123, word124,
  word125, word126, word127, word128, word129,
  word130, word131, word132, word133, word134,
  word135, word136, word137, word138, word139,
  word140, word141, word142, word143, word144,
  word145, word146, word147, word148, word149,
  word150, word151, word152, word153, word154,
  word155, word156, word157, word158, word159,
  word160, word161, word162, word163, word164,
  word165, word166, word167, word168, word169,
  word170, word171, word172, word173, word174,
  word175, word176, word177, word178, word179,
  word180, word181, word182, word183, word184,
  word185, word186, word187, word188, word189,
  word190, word191, word192, word193, word194,
  word195, word196, word197, word198, word199,
  word200, word201, word202, word203, word204,
  word205, word206, word207, word208, word209,
  word210, word211, word212, word213, word214,
  word215, word216, word217, word218, word219,
  word220, word221, word222, word223, word224,
  word225, word226, word227, word228, word229,
  word230, word231, word232, word233, word234,
  word235, word236, word237, word238, word239,
  word240, word241, word242, word243, word244,
  word245, word246, word247, word248, word249,
  word250, word251, word252, word253, word254,
  word255, word256, word257, word258, word259,
  word260, word261, word262, word263, word264,
  word265, word266, word267, word268, word269,
  word270, word271, word272, word273, word274,
  word275, word276, word277, word278, word279,
  word280, word281, word282, word283, word284,
  word285, word286, word287, word288, word289,
  word290, word291, word292, word293, word294,
  word295, word296, word297, word298, word299,
  word300, word301, word302, word303, word304,
  word305, word306, word307, word308, word309,
  word310, word311, word312, word313, word314,
  word315, word316, word317, word318, word319,
  word320, word321, word322, word323, word324,
  word325, word326, word327, word328, word329,
  word330, word331, word332, word333, word334,
  word335, word336, word337, word338, word339,
  word340, word341, word342, word343, word344,
  word345, word346, word347, word348, word349,
  word350, word351, word352, word353, word354,
  word355, word356, word357, word358, word359,
  word360, word361, word362, word363, word364,
  word365, word366, word367, word368, word369,
  word370, word371, word372, word373, word374,
  word375, word376, word377, word378, word379,
  word380, word381, word382, word383, word384,
  word385, word386, word387, word388, word389,
  word390, word391, word392, word393, word394,
  word395, word396, word397, word398, word399,
  word400, word401, word402, word403, word404,
  word405, word406, word407, word408, word409,
  word410, word411, word412, word413, word414,
  word415, word416, word417, word418, word419,
  word420, word421, word422, word423, word424,
  word425, word426, word427, word428, word429,
  word430, word431, word432, word433, word434,
  word435, word436, word437, word438, word439,
  word440, word441, word442, word443, word444,
  word445, word446, word447, word448, word449,
  word450, word451, word452, word453, word454,
  word455, word456, word457, word458, word459,
  word460, word461, word462, word463, word464,
  word465, word466, word467, word468, word469,
  word470, word471, word472, word473, word474,
  word475, word476, word477, word478, word479,
  word480, word481, word482, word483, word484,
  word485, word486, word487, word488, word489,
  word490, word491, word492, word493, word494,
  word495, word496, word497, word498, word499};

// Table of characters for random groups
const char CHAR_TABLE[40] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                              'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
                              'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', 
                              '4', '5', '6', '7', '8', '9', ',', '.', '/', '?'};
                              
// Global variable declarations
unsigned char i;
char buffer[BUFFER_SIZE];        // Type ahead and LCD buffer
unsigned char get_loc, put_loc;  // Buffer indices
unsigned char n_lcd_buffer;      // Number of char in buffer
char sc;

unsigned int elt_count;   // Delay loop counts per Morse element (dit)

//--------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)
// TIMER1 Compare A ISR
{
  OCR1A += TICKS_PER_HP; // Incr 16-bit reg OC1A = OC1AH and OC1AL
}

//--------------------------------------------------------------------
static inline void toggle_e()
{
  set_bit(LCD_PORT, E_BIT_NUM);    // Toggle the E pin on LCD
//  #if F_CPU > 4000000UL
//    _delay_us(0.5);
//  #endif
  clear_bit(LCD_PORT, E_BIT_NUM);
}

//--------------------------------------------------------------------
static void lcd_putchar(unsigned char lcd_data, unsigned char rs_value)
// Write data and RS value to LCD
// LCD is in 4 bit mode, 6 wire setup as per Exp. 46, Evil Genius
{
  LCD_PORT = (lcd_data >> 4) & 0x0F;  // Get high 4 bits for output
  LCD_PORT = LCD_PORT | ((rs_value & 1)*(1<<RS_BIT_NUM));
                                       // and set RS value
  toggle_e();                         // Toggle the high 4 bits out
  
  LCD_PORT = lcd_data & 0x0F;     // Get low 4 bits for output
  LCD_PORT = LCD_PORT | ((rs_value & 1)*(1<<RS_BIT_NUM));
                                  // and set RS value
  toggle_e();                     // Toggle the low 4 bits out
  
  if ((0 == (lcd_data & 0xFC)) && (0 == rs_value))
    _delay_loop_2(CNT_5MS);      // Delay 5 ms for certain commands
  else                        
    _delay_loop_2(CNT_200uS);    // Normal delay 200 us
}

//--------------------------------------------------------------------
static void lcd_init()
// Set LCD for 4 bit mode with 6 wire setup
{
  _delay_loop_2(CNT_20MS);  // Wait for LCD to power up

  LCD_PORT = 3;             // Start initialization process
  toggle_e();               // Send reset command
  _delay_loop_2(CNT_5MS);

  toggle_e();               // Repeat reset command
  _delay_loop_2(CNT_200uS); // but wait 200 us
  toggle_e();               // Repeat
  _delay_loop_2(CNT_200uS);

  LCD_PORT = 2;             // Put LCD into 4 bit mode
  toggle_e();
  _delay_loop_2(CNT_200uS);

  lcd_putchar(0b00101000, 0);  // LCD is 4 bit I/F, 2 line
  lcd_putchar(0b00000001, 0);  // Clear LCD 
  lcd_putchar(0b00000110, 0);  // Move cursor after each character
//lcd_putchar(0b00001110, 0);  // Turn on LCD and enable cursor 
  lcd_putchar(0b00001100, 0);  // Turn on LCD with disabled cursor
}
//--------------------------------------------------------------------
static void lcd_clear_line_2()
// Clear second line of LCD display
{
  unsigned char i;

  lcd_putchar(0b11000000, 0);   // Clear Line 2
  for (i=0; i<16; i++)
    lcd_putchar(32, 1);
}

//--------------------------------------------------------------------
static void lcd_clear_line_1()
// Clear first line of LCD display
{
  unsigned char i;

  lcd_putchar(0b10000000, 0);   // Clear Line 1
  for (i=0; i<16; i++)
    lcd_putchar(32, 1);
}

//--------------------------------------------------------------------
static char put_buffer(char entry) // Put entry in buffer
{
  if (((put_loc + 1) mod_buff_size) == get_loc)
    return 0;  // buffer full
  else
  {
    buffer[put_loc] = entry;
    put_loc = (put_loc + 1) mod_buff_size;
    return 1;
  }
}

//--------------------------------------------------------------------
static char get_buffer(void)  // Retrieve buffer entry
{
  char temp;
  if (get_loc == put_loc)
    return 0;  // buffer empty
  else
  {
    temp = buffer[get_loc];
    get_loc = (get_loc + 1) mod_buff_size;
    return temp;
  }
}

//--------------------------------------------------------------------
static unsigned char num_elt_buffer(void)
{
  if (put_loc >= get_loc)
    return put_loc - get_loc;
  else
    return BUFFER_SIZE - (get_loc - put_loc);
}

//--------------------------------------------------------------------
static void buffer_to_lcd(void)  // Write buffer to LCD
{
  unsigned char i;
  n_lcd_buffer = num_elt_buffer();
  for (i=0; i<BUFFER_SIZE; i++)
  {
    if (i == 0)
      lcd_putchar(0b10000000, 0);    //  Start line 1
    if (i == 16)
      lcd_putchar(0b11000000, 0);    //  Start line 2
    if (i < n_lcd_buffer)
      lcd_putchar(buffer[(get_loc + i) mod_buff_size], 1);
    else
      lcd_putchar(32, 1);  // Fill rest with blanks
  }
}

//--------------------------------------------------------------------
static void send_dit(void)  // Send dit
{
  KEY_ON;                    // Keyline output on
  SIDETONE_ON;               // Sidetone on
  _delay_loop_2(elt_count);  // Delay for one Morse element (dit)
  KEY_OFF;                   // output off
  SIDETONE_OFF;              // Sidetone off
  _delay_loop_2(elt_count);
}

//--------------------------------------------------------------------
static void send_dah(void)  // Send dah
{
  KEY_ON;                    // Keyline output on
  SIDETONE_ON;               // Sidetone on
  _delay_loop_2(elt_count);  // Delay for 3 Morse elements (dah)
  _delay_loop_2(elt_count);
  _delay_loop_2(elt_count);
  KEY_OFF;                   // output off
  SIDETONE_OFF;              // Sidetone off
  _delay_loop_2(elt_count);
}

//--------------------------------------------------------------------
static void send_char(char c)    // Send character
{
  unsigned char k, j, n_elts;
  char code;

  if (c == 32) k = 0;                          // Blank
  else if ((c > 38) && (c < 91)) k = c - 38;   // All others
  else k = 53;                                 // Send nothing
  code = CODE_TABLE[k];

  if (code == 1)  // For additional space btwn words
  { 
    _delay_loop_2(elt_count);
    _delay_loop_2(elt_count);
    _delay_loop_2(elt_count);
    _delay_loop_2(elt_count);
  }
  else if (code > 1)  // Send Morse character
  {
    for (j=0; j<8; j++)  // Find start bit
    {
      if ((code & 1) == 1) break;
      code = code >> 1;
    }
    code = code >> 1;  // Shift out start bit
    n_elts = 7 - j;    // Number of character elements

    for (j=0; j<n_elts; j++)  // Send character elements
    {
      if (code & 1) send_dit(); // 1 = dit
      else send_dah();          // 0 = dah
      code = code >> 1;        // Shift to next element
    }
    _delay_loop_2(elt_count);     // For additional space btwn char 
    _delay_loop_2(elt_count - 4*CNT_1MS); // subtract a few ms to 
                                  // allow for delay from LCD updating
  } 

}

//--------------------------------------------------------------------
static unsigned char PB_pushed(unsigned char BIT)
// Return 1 if pushed, otherwise return 0.
{
// Check for key push with debounce
  if (test_bit(PBPIN, BIT)) return 0;
  _delay_loop_2(CNT_20MS);        // Debounce delay
  if (test_bit(PBPIN, BIT)) return 0;

// Pushbutton still down after 20 ms.

// Debounce key release
  do
  {
    while (!(test_bit(PBPIN, BIT))) ; // Loop until release detected
    _delay_loop_2(CNT_20MS);          // Debounce delay
  } while (!(test_bit(PBPIN, BIT))) ; // Redo if release not sustained

  return 1;
}

//--------------------------------------------------------------------
int main(void)
{

  unsigned int k;   // Index for word addresses
  unsigned char wpm_tens_digit, wpm_ones_digit;

  unsigned int wpm;         // Words per minute Morse speed

  unsigned char state;      // Main state machine state
  unsigned char char_state; // State for character group selection menu

  unsigned int num_words;     // Size of current word list
  unsigned long divisor;      // Divisor for getting random index
  unsigned char num_chars;    // Number of characters being sent
  unsigned char start_chars;  // Starting index for characters


// Initialize LCD
  LCD_DDR = 0b00111111;
  lcd_init();

// Initialize pushbutton input pullups
  PBPORT |= ((1<<MENU_SELECT)|(1<<UP)|(1<<DOWN));

// Initialize keyline output
  set_bit(OUTDDR, OUTBIT);
  clear_bit(OUTPORT, OUTBIT);

// Initialize Morse speed and element count
  wpm = 25;
  elt_count = CNT_x_WPM / wpm ;

// Initialize word pool
  num_words = MAX_NUM_WORDS;
  divisor = RANDOM_MAX / ((long) num_words); 

// Initialize character pool
  num_chars = NUM_CHARS;
  divisor = RANDOM_MAX / ((long) NUM_CHARS);
  start_chars = 0;

// Initialize LCD circular FIFO buffer indices
  put_loc = 0;
  get_loc = 0;

// Set up TIMER1 to generate square wave sidetone on OC1A
  set_bit(SIDETONE_DDR, SIDETONE_BIT); // Set PB1 = OC1A for output
  TCCR1A = (1<<COM1A0);  // Set compare mode for toggle OC1A
  TCCR1B = (1<<CS00);    // Using system clock (no divide)
  TIMSK = (1<<OCIE1A);   // Unmask TIMER1 output compare A interrupt
  sei();  // Enable all interrupts
  SIDETONE_OFF;

// Send startup message in code
  _delay_loop_2(CNT_20MS);
  send_char('O');
  send_char('K');

// Start in menu selection state
  state = 2;

  while(1)  // Main loop
  {

    buffer_to_lcd();  // Display buffer contents

    if (PB_pushed(MENU_SELECT))    // Check MENU/SELECT button
      state = 2;                   // If pushed go to menu screens

    if (PB_pushed(DOWN))           // Check PAUSE/DOWN button
      while (!(PB_pushed(UP)));    // Pause if pushed until CONT/UP pushed

    switch (state)  // Main state machine
    {
      case 0:  // Normal send mode
        if (num_elt_buffer() > (BUFFER_SIZE / 2)) // If first line
          sc = get_buffer();    // full remove character from buffer
        if (num_elt_buffer() < (BUFFER_SIZE - MAX_WORD_LEN - 1))
        {     // If enough room, load next word in buffer
          k = (int)(random() / divisor);  // Random integer between 0
          if (k > (num_words - 1))         // and MAX_NUM_WORDS-1
            k = num_words - 1;
          for(i=0;; i++)
          {
            sc = pgm_read_string_byte(word_ptr[k], i) ;
            if (sc!=0) 
            {
              put_buffer(sc);
              buffer_to_lcd();
              if (sc) send_char(sc); // Not null, buffer not empty
            }
            else break;
          }
          put_buffer(' ');    // Trailing space
          buffer_to_lcd();
          send_char(' ');
        }
        break;

      case 1:   // Send groups of 5 random characters each
        if (num_elt_buffer() > (BUFFER_SIZE / 2)) // If first line
          sc = get_buffer();    // full remove character from buff
        if (num_elt_buffer() < (BUFFER_SIZE - GROUP_LEN - 1))
        {     // If enough room, load next group in buffer
          for(i=0; i<GROUP_LEN; i++)
          {
            k = random() / divisor;  // Random integer between 0
            if (k > (num_chars - 1))          // and num_chars-1
              k = num_chars - 1;
            k += start_chars;               // Add start index
            sc = CHAR_TABLE[k];
            put_buffer(sc);
            buffer_to_lcd();
            send_char(sc); // Not null, buffer not empty
          }
          put_buffer(' ');    // Trailing space
          buffer_to_lcd();
          send_char(' ');
        }
        break;

      case 2:   // Select speed, words or groups
        lcd_clear_line_1();
        lcd_clear_line_2();

        do      // Get new sending speed
        {
          lcd_putchar(0b10000000, 0);     // Start line 1
          for(i=0; msg1[i]!=0; i++)       // Print WPM
            lcd_putchar(msg1[i], 1);
          wpm_tens_digit = wpm / 10;
          wpm_ones_digit = wpm - (10 * wpm_tens_digit);
          lcd_putchar(wpm_tens_digit+'0', 1);
          lcd_putchar(wpm_ones_digit+'0', 1);

          if (PB_pushed(UP))              // Check for UP or DOWN
            if (wpm < MAX_WPM) wpm++;
          if (PB_pushed(DOWN))
            if (wpm > MIN_WPM) wpm--;

        } while (!(PB_pushed(MENU_SELECT)));
        elt_count = CNT_x_WPM / wpm;

        lcd_clear_line_1();
        lcd_clear_line_2();

        lcd_putchar(0b10000000, 0);     // Start line 1
        for(i=0; msg2[i]!=0; i++)       // Print SEND WORDS
          lcd_putchar(msg2[i], 1);
        state = 0;                      // Send words state

        do      // Choose words or character groups
        {
          if (PB_pushed(UP))              // Check for UP
          {
            lcd_putchar(0b10000000, 0);   // Start line 1
            for(i=0; msg3[i]!=0; i++)     // Print SEND GROUPS
              lcd_putchar(msg3[i], 1);
            state = 1;                    // Send groups state
          }

          if (PB_pushed(DOWN))
          {
            lcd_putchar(0b10000000, 0);   // Start line 1
            for(i=0; msg2[i]!=0; i++)     // Print SEND WORDS
              lcd_putchar(msg2[i], 1);
            state = 0;                    // Send words state
          }

        } while (!(PB_pushed(MENU_SELECT)));

        if (state == 0)
        {
          do
          {
            lcd_putchar(0b10000000, 0);     // Start line 1
            lcd_putchar((num_words/100)+'0', 1);
            for(i=0; msg4[i]!=0; i++)       // Print number of words
              lcd_putchar(msg4[i], 1);

            if (PB_pushed(UP))              // Check for UP or DOWN
              if (num_words < MAX_NUM_WORDS) num_words += 100;
            if (PB_pushed(DOWN))
              if (num_words > MIN_NUM_WORDS) num_words -= 100;

          } while (!(PB_pushed(MENU_SELECT)));
          divisor = RANDOM_MAX / ((long) num_words); 
        }

        if (state == 1)
        {
          char_state = 0;
          do
          {
            switch (char_state)
            {
              case 0:    // All characters
                lcd_putchar(0b10000000, 0);     // Start line 1
                for(i=0; msg5[i]!=0; i++)       // All characters
                  lcd_putchar(msg5[i], 1);
                num_chars = NUM_CHARS;
                start_chars = 0;
                divisor = RANDOM_MAX / ((long) NUM_CHARS);
                break;

              case 1:  // Letters only
                lcd_putchar(0b10000000, 0);     // Start line 1
                for(i=0; msg6[i]!=0; i++)       // Letters only
                  lcd_putchar(msg6[i], 1);
                num_chars = NUM_LETTERS;
                start_chars = 0;
                divisor = RANDOM_MAX / ((long) NUM_LETTERS);
                break;

              case 2:  // Numbers only
                lcd_putchar(0b10000000, 0);     // Start line 1
                for(i=0; msg7[i]!=0; i++) // Letters and numerals
                  lcd_putchar(msg7[i], 1);
                num_chars = NUM_NUMBERS;
                start_chars = NUM_LETTERS;
                divisor = RANDOM_MAX / ((long) NUM_NUMBERS);
                break;

              case 3:  // Letters and numbers only
                lcd_putchar(0b10000000, 0);     // Start line 1
                for(i=0; msg8[i]!=0; i++) // Letters and numerals
                  lcd_putchar(msg8[i], 1);
                num_chars = NUM_LET_AND_NUM;
                start_chars = 0;
                divisor = RANDOM_MAX / ((long) NUM_LET_AND_NUM);
                break;

              default:
                break;
            }
            if (PB_pushed(UP))           // Check for UP or DOWN
              if (char_state < MAX_CHAR_STATE) char_state++;
            if (PB_pushed(DOWN))
              if (char_state > MIN_CHAR_STATE) char_state--;

          } while (!(PB_pushed(MENU_SELECT)));
        }

        ATOMIC_BLOCK(ATOMIC_FORCEON)    // Reseed random num gen
        {
          srandom((long) OCR1A);
        }
        get_loc = 0;  // Flush buffer
        put_loc = 0;

        break;


      default:
        break;
    }

  }

}

 
