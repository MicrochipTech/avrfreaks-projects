# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# WinAVR Sample makefile written by Eric B. Weddington, Jörg Wunsch, et al.
# Released to the Public Domain
# Lire le manuel
#
# Additional material for this makefile was submitted by:
#  Tim Henigan
#  Peter Fleury
#  Reiner Patommel
#  Sander Pool
#  Frederik Rouleau
#  Markus Pfaff
# Traduit et rendu silencieux par Frank SAURET
#----------------------------------------------------------------------------
# Paramètre de la ligne de commande :
#
# make all = Lance make pour la compilation.
# La compilation est "intelligente" donc les fichiers qui n'ont pas changés ne sont pas recompilés. 
#
# make clean = Efface tous les fichiers générés par la compilation.
#
# make coff = Convertit ELF en AVR COFF (pour utiliser avec AVR Studio 3.x ou  VMLAB).
# 
# make extcoff = Convertit ELF en AVR COFF (pour utiliser avec AVR Studio 4.07 ou ultérieur).
#
# make program = charge le fichier hex dans le µC avec avrdude.  Il est possible
#                qu'il faille configurer avrdude aussi (normalement c'est fait).
#
# make debug = Démarre soit simulavr, soit avarice pour debugger avec soit avr-gdb soit avr-insight comme interface.
#
# make NomDeFichier.s = Compile seulement  NomDeFichier.en assembleur
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
#make reconstruire = efface toute compilation antérieure et recompile tout (Ajouté par F.SAURET)
#----------------------------------------------------------------------------

#Obsolète. Laisser à 1.
AVRStudioExclusif = 1

# Indiquer ici si on veut créer le coff lor de la comiplation (nécessite le paramètre stabs
# plus bas (dans DEBUG). Valeurs : coff ou extcoff ou rien
CompilCoff = 


# Format de déboggage.
# Les format natifs pour AVR-GCC's -g sont stabs , ou dwarf-2 [default].
#AVR Studio 4.10 requière dwarf-2
#Protéus requière ExtCoff
# AVR (extended) COFF requière stabs, plus l'utilisation d'avr-objcopy.
DEBUG = dwarf-2

# Permet de demander l'affichage de la mémoire occupée. Valeurs :  OccupationMemoire ou rien
AffichageMemoire = OccupationMemoire

#Création de la ligne de la commande all
ChaineCompilation=build $(CompilCoff) finished $(AffichageMemoire)

# Nom du µC
MCU = attiny26

# Fréquence processeur.
#     Définit une variable F_CPU, dans tous les sources comme la fréquence du processeur 
#     Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
F_CPU = 16000000

# Format de sortie (srec, ihex ou binary)
FORMAT = ihex

# Fichier de sortie (sans extension et il doit être le même que le fichier C).
TARGET = M_BUS

# Liste des fichiers C à compiler. Séparé par un espace (Les dépendances sont automatiquement générées.)
SRC = $(TARGET)_data.c send.c ping_ok.c Compute_CRC.c init.c send_frame.c interrupts.c Ack_Play_State.c Ack_Wait.c Ack_Change_Disk.c Ack_Disk_Info.c Decode_Received.c loop_ms.c time_loop.c Ack_Power_Up.c $(TARGET).c

#  Liste des fichiers assembleur.
# Ici vous pouvez rajouter un fichier en assembleur mais qui doit avoir l'extension .S 
# avec un S majuscule. ex : ASRC = myasmfile.S
ASRC = 

# Type d'optimissation [0, 1, 2, 3, s]. 
# 0 = pas d'optimisation. s = Optimise la taille.
# 1, 2, 3 Optimise la vitesse. (3 n'est pas tjs le plus rapide. Voir avr-libc FAQ.)
OPT = s

# Répertoires supplémentaires pour les fichier includes.
# Séparés par un espace
EXTRAINCDIRS =  C:/Elec/WinAVR/avr/include

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)UL

# Place -I options here
CINCS =

#---------------- Options de compilation ----------------
#  -g*:          Génére les information de déboggage
#  -O*:          Type d'optimisation
#  -f...:           tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)

#---------------- Options d'assemblage ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 

#---------------- Librairies ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm

#------------------- Options pour la mémoire externe ------------------------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#----------------  Options de chainage ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

#----------------  Programmation avec AVRDUDE ----------------
# Programmateur : alf avr910 avrisp bascom bsd 
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Taper : avrdude -c ? Pour avoir la liste complète
#
AVRDUDE_PROGRAMMER = stk200

# Port du programmateur (com1, com2 ... lpt1, lpt2 …). 
AVRDUDE_PORT = lpt1

#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex:i

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.

#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p t26 -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)
AVRDUDE_FLAGS += -E noreset

#---------------- Options de debbugage ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Commenter la ligne non désirée.
# DEBUG_UI = gdb
DEBUG_UI = insight

# Commenter la ligne non désirée.
#DEBUG_BACKEND = avarice
DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when 
#     avarice is running on a different computer.
DEBUG_HOST = localhost


#============================================================================

# Définir les répertoires si nécessaire.
#DIRAVR = c:/elec/winavr
#DIRAVRBIN = $(DIRAVR)/bin
#DIRAVRUTILS = $(DIRAVR)/utils/bin
#DIRINC = .
#DIRLIB = $(DIRAVR)/avr/lib

#Définir les programmes et les commandes.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
WINSHELL = cmd

# Messages Français : Attention pas d'accents ni apostrophe
 MSG_ERRORS_NONE = ++++++++++ Aucune erreur ++++++++++
 MSG_BEGIN = -------- Debut  --------
 MSG_END = --------  Fin   --------
 MSG_SIZE_BEFORE = Taille avant : 
 MSG_SIZE_AFTER = Taille apres :
 MSG_COFF = Convertit en  AVR COFF :
 MSG_EXTENDED_COFF = Convertit en  AVR Extended COFF :
 MSG_FLASH = Cree le fichier de chargement de la flash :
 MSG_EEPROM = Cree le fichier de chargement de l EEPROM :
 MSG_EXTENDED_LISTING = Cree le Listing etendu :
 MSG_SYMBOL_TABLE = Cree la Table de symbole :
 MSG_LINKING = Linkage :
 MSG_COMPILING = Compilation :
 MSG_ASSEMBLING = Assemblage :
 MSG_CLEANING = Nettoyage du projet :
 MSG_FIN_NETTOYAGE = Nettoyage finit !

# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)

# Compiler flags to generate dependency files.
#Ancienne ligne voir ce quelle veut dire : 
#GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

#----------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------

# Default target.
all:  $(ChaineCompilation)

build: elf hex eep lss sym  

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

finished:
	@echo
	@echo $(MSG_ERRORS_NONE)
	@echo
	
end:
	@echo $(MSG_END)

# Affiche la taille des fichiers.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
AVRMEM = avr-mem.sh $(TARGET).elf $(MCU)

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	$(AVRMEM) 2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	$(AVRMEM) 2>/dev/null; echo; fi

OccupationMemoire:	
	@if test -f $(TARGET).elf; then echo; $(AVRMEM) 2>/dev/null; echo; fi
	
# Affiche la version du compilateur
gccversion : 
	@$(CC) --version

# Programme le µC. 
program: $(TARGET).hex $(TARGET).eep
	@$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set 
#     a breakpoint at main().
gdb-config: 
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif	
	@echo break main >> $(GDBINIT_FILE)
	
debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause
	
else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)
	
# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 

coff: $(TARGET).elf
#	@echo
#	@echo $(MSG_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-avr $< $(TARGET).cof

extcoff: $(TARGET).elf
#	@echo
#	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
#	@echo
#	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
#	@echo
#	@echo $(MSG_EEPROM) $@
	@-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
#	@echo
#	@echo $(MSG_EXTENDED_LISTING) $@
	@$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
#	@echo
#	@echo $(MSG_SYMBOL_TABLE) $@
	@$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
#	@echo
#	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(ALL_CFLAGS) $< -o $@ 
	@echo

# Compile: create assembler files from C source files.
%.s : %.c
	@$(CC) -S $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
%.o : %.S
#	@echo
#	@echo $(MSG_ASSEMBLING) $<
	@$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@ 
	
# Target: clean project.
clean: clean_list  

clean_list :
#	@echo
#	@echo $(MSG_CLEANING)
	@$(REMOVE) $(TARGET).hex
	@$(REMOVE) $(TARGET).eep
	@$(REMOVE) $(TARGET).obj
	@$(REMOVE) $(TARGET).cof
	@$(REMOVE) $(TARGET).elf
	@$(REMOVE) $(TARGET).map
	@$(REMOVE) $(TARGET).obj
	@$(REMOVE) $(TARGET).a90
	@$(REMOVE) $(TARGET).sym
	@$(REMOVE) $(TARGET).lnk
	@$(REMOVE) $(TARGET).lss
	@$(REMOVE) $(OBJ)
	@$(REMOVE) $(LST)
	@$(REMOVE) $(SRC:.c=.s)
	@$(REMOVE) $(SRC:.c=.d)
	@$(REMOVE) .dep/*
	@echo
	@echo $(MSG_FIN_NETTOYAGE)
	@echo

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# Role : Reconstruire le projet
reconstruire: clean all

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program reconstruire


