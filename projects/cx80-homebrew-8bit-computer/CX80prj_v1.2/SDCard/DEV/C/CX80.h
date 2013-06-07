#ifndef _CX80_H
#define _CX80_H

#define CX80_HiByte(x)                      (unsigned char)(x >> 8)
#define CX80_LoByte(x)                      (unsigned char)(x & 0xFF)

#define CX80_SYSTEM_FNC_REG_ADDR            0x0000
#define CX80_SYSTEM_REG1L_ADDR              0x0001
#define CX80_SYSTEM_REG1H_ADDR              0x0002
#define CX80_SYSTEM_REG2L_ADDR              0x0003
#define CX80_SYSTEM_REG2H_ADDR              0x0004
#define CX80_SYSTEM_REG3L_ADDR              0x0005
#define CX80_SYSTEM_REG3H_ADDR              0x0006
#define CX80_SYSTEM_CALL_FNC_ADDR           0x0007

#define CX80_FNC_PRINT_STRING               0x00
#define CX80_FNC_PRINT_CHAR                 0x01
#define CX80_FNC_CURSOR_UP                  0x02
#define CX80_FNC_CURSOR_DOWN                0x03
#define CX80_FNC_CURSOR_RIGHT               0x04
#define CX80_FNC_CURSOR_LEFT                0x05
#define CX80_FNC_CLEAR_SCREEN               0x06
#define CX80_FNC_CURSOR_HOME                0x07
#define CX80_FNC_REVERSE_LINE_FEED          0x08
#define CX80_FNC_CLEAR_TO_END_OF_SCREEN     0x09
#define CX80_FNC_CLEAR_TO_END_OF_LINE       0x0A
#define CX80_FNC_MOVE_CURSOR                0x0B
#define CX80_FNC_SET_LINE_DIMENSION         0x0C
#define CX80_FNC_CLEAR_TO_START_OF_SCREEN   0x0D
#define CX80_FNC_CURSOR_SHOW                0x0E
#define CX80_FNC_SAVE_CURSOR_POSITION       0x0F
#define CX80_FNC_RESTORE_CURSOR_POSITION    0x10
#define CX80_FNC_CLEAR_LINE                 0x11
#define CX80_FNC_CLEAR_TO_START_OF_LINE     0x12
#define CX80_FNC_LINE_OVERFLOW              0x13
#define CX80_FNC_BLOCK_CURSOR               0x14
#define CX80_FNC_UNDERLINE_CURSOR           0x15
#define CX80_FNC_INVERT_VIDEO_OUTPUT        0x16

#define CX80_FNC_OPEN_CREATE_FILE           0x17
#define CX80_FNC_READ_BYTE_FROM_FILE        0x18
#define CX80_FNC_WRITE_BYTE_TO_FILE         0x19
#define CX80_FNC_CLOSE_FILE                 0x1A
#define CX80_FNC_FILE_SEEK                  0x1B
#define CX80_FNC_READ_FILE_BUFFER           0x1C
#define CX80_FNC_WRITE_FILE_BUFFER          0x1D
#define CX80_FNC_CHANGE_DIRECTORY           0x1E
#define CX80_FNC_CREATE_DIRECTORY           0x1F
#define CX80_FNC_DELETE_FILE_DIRECTORY      0x20
#define CX80_FNC_RENAME_FILE_DIRECTORY      0x21
#define CX80_FNC_GET_CURRENT_DIRECTORY      0x22
#define CX80_FNC_GET_STRING_FROM_FILE       0x2A

#define CX80_FNC_RETURN_KEY_PRESSED         0x23
#define CX80_FNC_WAIT_FOR_KEYPRESSED        0x24
#define CX80_FNC_MILLISECONDS_DELAY         0x25
#define CX80_FNC_GET_INPUT_STRING           0x26
#define CX80_FNC_PLAY_PIEZO_SOUND           0x27
#define CX80_FNC_WRITE_IO_PORT              0x28
#define CX80_FNC_READ_IO_PORT               0x29
#define CX80_FNC_GET_OS_COMMAND_LINE        0x2B
#define CX80_FNC_GET_OS_COMMAND_LINE_PARAM  0x2C

#define CX80_FNC_STOP_USER_PROGRAM          0xFF

#define CX80_DO_SYSTEM_CALL                 __asm__("call #0x0007");

#define FA_READ                             1
#define FA_WRITE                            2
#define FA_CREATE_ALWAYS                    8

#endif