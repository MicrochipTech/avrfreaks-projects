############################################################################################
# Makefile for the project MAC_Example_1_Nobeacon Release Using single source files
############################################################################################
# $Id: Makefile 14244 2009-03-12 16:20:41Z sschneid $
### Modified for Teensy2 9/18/2010    -jkl
### Disabled IEEE Address check		-jkl

# Build specific properties
_TAL_TYPE = AT86RF230B
_PAL_TYPE = ATMEGA32U4
_PAL_GENERIC_TYPE = AVR
_BOARD_TYPE = T2
_HIGHEST_STACK_LAYER = MAC

# Path variables
## Path to main project directory
MAIN_DIR = ../../../../../..
APP_DIR = ../..
PATH_APP = $(MAIN_DIR)/Applications
PATH_TAL = $(MAIN_DIR)/TAL
PATH_MAC = $(MAIN_DIR)/MAC
PATH_TAL_CB = $(MAIN_DIR)/TAL/Src
PATH_PAL = $(MAIN_DIR)/PAL
PATH_RES = $(MAIN_DIR)/Resources
PATH_GLOB_INC = $(MAIN_DIR)/Includes

## General Flags
PROJECT = Device
MCU = atmega32u4
TARGET_DIR = .
TARGET = $(TARGET_DIR)/$(PROJECT).elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -Werror -g -Wundef -std=c99 -Os
CFLAGS += -DDEBUG=0
CFLAGS += -DREDUCED_PARAM_CHECK
CFLAGS += -DDISABLE_IEEE_ADDR_CHECK
CFLAGS += -DTAL_TYPE=$(_TAL_TYPE)
CFLAGS += -DPAL_GENERIC_TYPE=$(_PAL_GENERIC_TYPE)
CFLAGS += -DPAL_TYPE=$(_PAL_TYPE)
CFLAGS += -DBOARD_TYPE=$(_BOARD_TYPE)
CFLAGS += -DHIGHEST_STACK_LAYER=$(_HIGHEST_STACK_LAYER)
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-g

## Linker flags
LDFLAGS = $(COMMON) -Wl,-Map=$(PROJECT).map

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Include directories for application
INCLUDES = -I $(APP_DIR)/Inc
## Include directories for general includes
INCLUDES += -I $(MAIN_DIR)/Include
## Include directories for resources
INCLUDES += -I $(MAIN_DIR)/Resources/Buffer_Management/Inc/
INCLUDES += -I $(MAIN_DIR)/Resources/Queue_Management/Inc/
## Include directories for MAC
INCLUDES += -I $(MAIN_DIR)/MAC/Inc/
## Include directories for TAL
INCLUDES += -I $(MAIN_DIR)/TAL/Inc/
INCLUDES += -I $(MAIN_DIR)/TAL/$(_TAL_TYPE)/Inc/
## Include directories for PAL
INCLUDES += -I $(MAIN_DIR)/PAL/Inc/
INCLUDES += -I $(MAIN_DIR)/PAL/$(_PAL_GENERIC_TYPE)/Generic/Inc
INCLUDES += -I $(MAIN_DIR)/PAL/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Inc/
## Include directories for specific boards type
INCLUDES += -I $(MAIN_DIR)/PAL/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Boards/
INCLUDES += -I $(MAIN_DIR)/PAL/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Boards/$(_BOARD_TYPE)

## Objects that must be built in order to link
### Deleted:
### $(TARGET_DIR)/pal_uart.o
###  $(TARGET_DIR)/pal_sio_hub.o
OBJECTS = $(TARGET_DIR)/main.o\
	$(TARGET_DIR)/pal_irq.o\
	$(TARGET_DIR)/pal.o\
	$(TARGET_DIR)/pal_timer.o\
	$(TARGET_DIR)/pal_board.o\
	$(TARGET_DIR)/pal_utils.o\
	$(TARGET_DIR)/pal_trx_access.o\
	$(TARGET_DIR)/bmm.o\
	$(TARGET_DIR)/qmm.o\
	$(TARGET_DIR)/tal.o\
	$(TARGET_DIR)/tal_rx.o\
	$(TARGET_DIR)/tal_tx.o\
	$(TARGET_DIR)/tal_ed.o\
	$(TARGET_DIR)/tal_slotted_csma.o\
	$(TARGET_DIR)/tal_pib.o\
	$(TARGET_DIR)/tal_init.o\
	$(TARGET_DIR)/tal_irq_handler.o\
	$(TARGET_DIR)/tal_pwr_mgmt.o\
	$(TARGET_DIR)/tal_rx_enable.o \
	$(TARGET_DIR)/mac_associate.o \
	$(TARGET_DIR)/mac_beacon.o \
	$(TARGET_DIR)/mac_callback_wrapper.o \
	$(TARGET_DIR)/mac_data_ind.o \
	$(TARGET_DIR)/mac_data_req.o \
	$(TARGET_DIR)/mac_disassociate.o \
	$(TARGET_DIR)/mac_dispatcher.o \
	$(TARGET_DIR)/mac.o \
	$(TARGET_DIR)/mac_mcps_data.o \
	$(TARGET_DIR)/mac_misc.o \
	$(TARGET_DIR)/mac_orphan.o \
	$(TARGET_DIR)/mac_pib.o \
	$(TARGET_DIR)/mac_poll.o \
	$(TARGET_DIR)/mac_process_beacon_frame.o \
	$(TARGET_DIR)/mac_process_tal_tx_frame_status.o \
	$(TARGET_DIR)/mac_rx_enable.o \
	$(TARGET_DIR)/mac_scan.o \
	$(TARGET_DIR)/mac_start.o \
	$(TARGET_DIR)/mac_sync.o \
	$(TARGET_DIR)/mac_tx_coord_realignment_command.o \
	$(TARGET_DIR)/mac_api.o \
	$(TARGET_DIR)/usr_mcps_purge_conf.o \
	$(TARGET_DIR)/usr_mcps_data_ind.o \
	$(TARGET_DIR)/usr_mlme_beacon_notify_ind.o \
	$(TARGET_DIR)/usr_mlme_disassociate_conf.o \
	$(TARGET_DIR)/usr_mlme_disassociate_ind.o \
	$(TARGET_DIR)/usr_mlme_get_conf.o \
	$(TARGET_DIR)/usr_mlme_set_conf.o \
	$(TARGET_DIR)/usr_mlme_orphan_ind.o \
	$(TARGET_DIR)/usr_mlme_poll_conf.o \
	$(TARGET_DIR)/usr_mlme_rx_enable_conf.o \
	$(TARGET_DIR)/usr_mlme_sync_loss_ind.o

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Build
all: $(TARGET) $(TARGET_DIR)/$(PROJECT).hex $(TARGET_DIR)/$(PROJECT).eep $(TARGET_DIR)/$(PROJECT).lss size

## Compile
### Deleted:
### $(TARGET_DIR)/pal_uart.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/Generic/Src/pal_uart.c
### $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
### $(TARGET_DIR)/pal_sio_hub.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Src/pal_sio_hub.c
### $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/main.o: $(APP_DIR)/Src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  -o $@ $<
$(TARGET_DIR)/pal_irq.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Boards/$(_BOARD_TYPE)/pal_irq.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/pal.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/Generic/Src/pal.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/pal_timer.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/Generic/Src/pal_timer.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/pal_board.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/$(_PAL_TYPE)/Boards/$(_BOARD_TYPE)/pal_board.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/pal_utils.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/Generic/Src/pal_utils.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/pal_trx_access.o: $(PATH_PAL)/$(_PAL_GENERIC_TYPE)/Generic/Src/pal_trx_access.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/bmm.o: $(PATH_RES)/Buffer_Management/Src/bmm.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/qmm.o: $(PATH_RES)/Queue_Management/Src/qmm.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_rx.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_rx.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_tx.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_tx.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_ed.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_ed.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_slotted_csma.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_slotted_csma.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_pib.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_pib.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_init.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_init.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_irq_handler.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_irq_handler.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_pwr_mgmt.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_pwr_mgmt.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/tal_rx_enable.o: $(PATH_TAL)/$(_TAL_TYPE)/Src/tal_rx_enable.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_associate.o: $(PATH_MAC)/Src/mac_associate.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_beacon.o: $(PATH_MAC)/Src/mac_beacon.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_callback_wrapper.o: $(PATH_MAC)/Src/mac_callback_wrapper.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_data_ind.o: $(PATH_MAC)/Src/mac_data_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_data_req.o: $(PATH_MAC)/Src/mac_data_req.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_disassociate.o: $(PATH_MAC)/Src/mac_disassociate.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_dispatcher.o: $(PATH_MAC)/Src/mac_dispatcher.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac.o: $(PATH_MAC)/Src/mac.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_mcps_data.o: $(PATH_MAC)/Src/mac_mcps_data.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_misc.o: $(PATH_MAC)/Src/mac_misc.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_orphan.o: $(PATH_MAC)/Src/mac_orphan.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_pib.o: $(PATH_MAC)/Src/mac_pib.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_poll.o: $(PATH_MAC)/Src/mac_poll.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_process_beacon_frame.o: $(PATH_MAC)/Src/mac_process_beacon_frame.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_process_tal_tx_frame_status.o: $(PATH_MAC)/Src/mac_process_tal_tx_frame_status.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_rx_enable.o: $(PATH_MAC)/Src/mac_rx_enable.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_scan.o: $(PATH_MAC)/Src/mac_scan.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_start.o: $(PATH_MAC)/Src/mac_start.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_sync.o: $(PATH_MAC)/Src/mac_sync.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_tx_coord_realignment_command.o: $(PATH_MAC)/Src/mac_tx_coord_realignment_command.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mcps_data_conf.o: $(PATH_MAC)/Src/usr_mcps_data_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mcps_data_ind.o: $(PATH_MAC)/Src/usr_mcps_data_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mcps_purge_conf.o: $(PATH_MAC)/Src/usr_mcps_purge_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_associate_conf.o: $(PATH_MAC)/Src/usr_mlme_associate_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_associate_ind.o: $(PATH_MAC)/Src/usr_mlme_associate_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_beacon_notify_ind.o: $(PATH_MAC)/Src/usr_mlme_beacon_notify_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_comm_status_ind.o: $(PATH_MAC)/Src/usr_mlme_comm_status_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_disassociate_conf.o: $(PATH_MAC)/Src/usr_mlme_disassociate_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_disassociate_ind.o: $(PATH_MAC)/Src/usr_mlme_disassociate_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_get_conf.o: $(PATH_MAC)/Src/usr_mlme_get_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_orphan_ind.o: $(PATH_MAC)/Src/usr_mlme_orphan_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_poll_conf.o: $(PATH_MAC)/Src/usr_mlme_poll_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_reset_conf.o: $(PATH_MAC)/Src/usr_mlme_reset_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_rx_enable_conf.o: $(PATH_MAC)/Src/usr_mlme_rx_enable_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_scan_conf.o: $(PATH_MAC)/Src/usr_mlme_scan_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_set_conf.o: $(PATH_MAC)/Src/usr_mlme_set_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_start_conf.o: $(PATH_MAC)/Src/usr_mlme_start_conf.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/usr_mlme_sync_loss_ind.o: $(PATH_MAC)/Src/usr_mlme_sync_loss_ind.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
$(TARGET_DIR)/mac_api.o: $(PATH_MAC)/Src/mac_api.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## avr-size options
IS_WIN32 := $(shell uname -s | sed -n -e 's/^MINGW.*/-C/p' -e 's/^CYGWIN.*/-C/p')
ifdef IS_WIN32
SIZEFLAGS = -C --mcu=${MCU}
else
SIZEFLAGS = -B
endif

size: ${TARGET}
	@echo
	@avr-size $(SIZEFLAGS) ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(TARGET_DIR)/*.o $(TARGET_DIR)/$(PROJECT).elf dep/* $(TARGET_DIR)/$(PROJECT).hex $(TARGET_DIR)/$(PROJECT).eep $(TARGET_DIR)/$(PROJECT).lss $(TARGET_DIR)/$(PROJECT).map

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

