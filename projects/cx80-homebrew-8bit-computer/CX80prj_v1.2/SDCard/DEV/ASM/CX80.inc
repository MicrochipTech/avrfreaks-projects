;CX80 System Calls definition for zDevStudio assembler ide.
;
;To download the free zDevStudio ide go to: http://sourceforge.net/projects/zdevstudio/

CX80_SYSTEM_FNC_REG_ADDR                    equ $0000
CX80_SYSTEM_REG1L_ADDR                      equ $0001
CX80_SYSTEM_REG1H_ADDR                      equ $0002
CX80_SYSTEM_REG2L_ADDR                      equ $0003
CX80_SYSTEM_REG2H_ADDR                      equ $0004
CX80_SYSTEM_REG3L_ADDR                      equ $0005
CX80_SYSTEM_REG3H_ADDR                      equ $0006
CX80_SYSTEM_CALL_FNC_ADDR                   equ $0007

CX80_FNC_PRINT_STRING                       equ $00
CX80_FNC_PRINT_CHAR                         equ $01
CX80_FNC_CURSOR_UP                          equ $02
CX80_FNC_CURSOR_DOWN                        equ $03
CX80_FNC_CURSOR_RIGHT                       equ $04
CX80_FNC_CURSOR_LEFT                        equ $05
CX80_FNC_CLEAR_SCREEN                       equ $06
CX80_FNC_CURSOR_HOME                        equ $07
CX80_FNC_REVERSE_LINE_FEED                  equ $08
CX80_FNC_CLEAR_TO_END_OF_SCREEN             equ $09
CX80_FNC_CLEAR_TO_END_OF_LINE               equ $0A
CX80_FNC_MOVE_CURSOR                        equ $0B
CX80_FNC_SET_LINE_DIMENSION                 equ $0C
CX80_FNC_CLEAR_TO_START_OF_SCREEN           equ $0D
CX80_FNC_CURSOR_SHOW                        equ $0E
CX80_FNC_SAVE_CURSOR_POSITION               equ $0F
CX80_FNC_RESTORE_CURSOR_POSITION            equ $10
CX80_FNC_CLEAR_LINE                         equ $11
CX80_FNC_CLEAR_TO_START_OF_LINE             equ $12
CX80_FNC_LINE_OVERFLOW                      equ $13
CX80_FNC_BLOCK_CURSOR                       equ $14
CX80_FNC_UNDERLINE_CURSOR                   equ $15
CX80_FNC_INVERT_VIDEO_OUTPUT                equ $16

CX80_FNC_OPEN_CREATE_FILE                   equ $17
CX80_FNC_READ_BYTE_FROM_FILE                equ $18
CX80_FNC_WRITE_BYTE_TO_FILE                 equ $19
CX80_FNC_CLOSE_FILE                         equ $1A
CX80_FNC_FILE_SEEK                          equ $1B
CX80_FNC_READ_FILE_BUFFER                   equ $1C
CX80_FNC_WRITE_FILE_BUFFER                  equ $1D
CX80_FNC_CHANGE_DIRECTORY                   equ $1E
CX80_FNC_CREATE_DIRECTORY                   equ $1F
CX80_FNC_DELETE_FILE_DIRECTORY              equ $20
CX80_FNC_RENAME_FILE_DIRECTORY              equ $21
CX80_FNC_GET_CURRENT_DIRECTORY              equ $22
CX80_FNC_GET_STRING_FROM_FILE               equ $2A

CX80_FNC_RETURN_KEY_PRESSED                 equ $23
CX80_FNC_WAIT_FOR_KEYPRESSED                equ $24
CX80_FNC_MILLISECONDS_DELAY                 equ $25
CX80_FNC_GET_INPUT_STRING                   equ $26
CX80_FNC_PLAY_PIEZO_SOUND                   equ $27
CX80_FNC_WRITE_IO_PORT                      equ $28
CX80_FNC_READ_IO_PORT                       equ $29
CX80_FNC_GET_OS_COMMAND_LINE                equ $2B
CX80_FNC_GET_OS_COMMAND_LINE_PARAM          equ $2C

CX80_FNC_STOP_USER_PROGRAM                  equ $FF

FA_READ                                     equ 1
FA_WRITE                                    equ 2
FA_CREATE_ALWAYS                            equ 8
