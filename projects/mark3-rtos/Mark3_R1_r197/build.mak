#----------------------------------------------------------------------------
# Mark3 Makefile system
# Copyright (c) 2012 Funkenstein Software Consulting
#----------------------------------------------------------------------------

# The user specifies objects in the OBJ variable
OBJ_DIR_FINAL=$(OBJ_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)/
APP_DIR_FINAL=$(APP_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)/
LIB_DIR_FINAL=$(LIB_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)/

C_OBJ=$(addprefix $(OBJ_DIR_FINAL), $(C_SOURCE:%.c=%.c.o))
CPP_OBJ=$(addprefix $(OBJ_DIR_FINAL), $(CPP_SOURCE:%.cpp=%.cpp.o))

PORT_DIR=./cpu/$(ARCH)/$(VARIANT)/$(TOOLCHAIN)/
PORT_OBJ_DIR=$(PORT_DIR)obj/

PORT_C_SOURCE_ADJ=$(addprefix $(PORT_DIR), $(PORT_C_SOURCE))
PORT_CPP_SOURCE_ADJ=$(addprefix $(PORT_DIR), $(PORT_CPP_SOURCE))

PORT_C_OBJ=$(addprefix $(PORT_OBJ_DIR), $(PORT_C_SOURCE:%.c=%.c.o))
PORT_CPP_OBJ=$(addprefix $(PORT_OBJ_DIR), $(PORT_CPP_SOURCE:%.cpp=%.cpp.o))

USR_OBJS=$(C_OBJ) $(CPP_OBJ) $(PORT_C_OBJ) $(PORT_CPP_OBJ)

CPU_SPEC_HEADERS=$(PORT_DIR)/public/

# Static analysis output
C_SA_OBJ=$(addprefix $(OBJ_DIR_FINAL), $(C_SOURCE:%.c=%.c.sa))
CPP_SA_OBJ=$(addprefix $(OBJ_DIR_FINAL), $(CPP_SOURCE:%.cpp=%.cpp.sa))

PORT_C_SA_OBJ=$(addprefix $(PORT_OBJ_DIR), $(PORT_C_SOURCE:%.c=%.c.sa))
PORT_CPP_SA_OBJ=$(addprefix $(PORT_OBJ_DIR), $(PORT_CPP_SOURCE:%.cpp=%.cpp.sa))

USR_SA_OBJS=$(C_SA_OBJ) $(CPP_SA_OBJ) $(PORT_C_SA_OBJ) $(PORT_CPP_SA_OBJ)

ifeq ($(IS_APP), 1)
	LFLAGS+=-Wl,-Map="$(APP_NAME).map"
endif

# the target is specified in the user makefile
all : banner binary 

# Display what we're building
clean_banner : $(realpath .)
	@echo ------------------------------------------------------------
	@echo [ Cleaning $^ ]
	@echo ------------------------------------------------------------
	
# Display what we're building
banner : $(realpath .)
	@echo ------------------------------------------------------------
	@echo [ Building $^ ]
	@echo ------------------------------------------------------------

# Display what we're building
headers_banner : $(realpath .)
	@echo ------------------------------------------------------------
	@echo [ Headers $^ ]
	@echo ------------------------------------------------------------

# Display what we're building
source_banner : $(realpath .)
	@echo ------------------------------------------------------------
	@echo [ Source $^ ]
	@echo ------------------------------------------------------------
	
# Get rid of all of the binary objects created previous... 
clean : clean_banner
	 $(RMCMD) $(USR_SA_OBJS)
	 $(RMCMD) $(USR_OBJS)
ifeq ($(IS_LIB),1)
	 $(RMCMD) $(OBJ_DIR_FINAL)lib$(LIBNAME).a
endif
ifeq ($(IS_APP),1)
	 $(RMCMD) $(OBJ_DIR_FINAL)$(APPNAME).elf
	 $(RMCMD) $(OBJ_DIR_FINAL)$(APPNAME).hex
endif
# Recursive cleaning...
	@ for i in $(SUBDIRS); \
	do \
		if test -f $$i/makefile; then \
			cd $$i ;\
			$(MAKE) clean ;\
			cd .. ;\
		fi ;\
	done

#----------------------------------------------------------------------------
# Copy public header files to the staging directory
#----------------------------------------------------------------------------
headers : headers_banner
	@if test -d $(PUBLIC_DIR); then \
		if test -d $(INC_DIR); then \
			if test -f $(wildcard *.h); then \
				$(COPYCMD) $(PUBLIC_DIR)*.h $(INC_DIR); \
			fi;	\
		fi; \
	fi;
	@if test -d $(CPU_SPEC_HEADERS); then \
		if test -d $(INC_DIR); then \
			if test -f $(wildcard *.h); then \
			    $(COPYCMD) $(CPU_SPEC_HEADERS)*.h $(INC_DIR); \
			fi;	\
		fi; \
	fi;
	@for i in $(SUBDIRS); \
	do \
		if test -f $$i/makefile; then \
			cd $$i ;\
			$(MAKE) headers ;\
			cd .. ;\
		fi; \
	done

#----------------------------------------------------------------------------
# Copy source files to the staging directory
#----------------------------------------------------------------------------
source: source_banner
ifneq ($(wildcard *.cpp), )
		@$(COPYCMD) *.cpp $(SRC_DIR)
endif
ifneq ($(wildcard *.c), )
		@$(COPYCMD) *.c $(SRC_DIR)
endif
ifneq ($(wildcard *.h), )
		@$(COPYCMD) *.h $(SRC_DIR)
endif
	@if test -d $(PUBLIC_DIR); then \
		if test -d $(INC_DIR); then \
			if test -f $(wildcard *.h); then \
				$(COPYCMD) $(PUBLIC_DIR)*.h $(SRC_DIR); \
			fi;	\
		fi; \
	fi;
	@for i in $(SUBDIRS); \
	do \
		if test -f $$i/makefile; then \
			cd $$i ;\
			$(MAKE) source ;\
			cd .. ;\
		fi; \
	done
	
	
#----------------------------------------------------------------------------
# Build libraries from objects
#----------------------------------------------------------------------------
ifeq ($(IS_LIB), 1)
library : banner directories objects $(USR_OBJS) static_data $(USR_SA_OBJS)
else
library : banner
endif
ifeq ($(IS_LIB), 1)
	@echo LIBRARY: $(LIBNAME)
	$(AR) $(ARFLAGS) $(OBJ_DIR_FINAL)lib$(LIBNAME).a $(USR_OBJS)
	-cat $(USR_SA_OBJS) > lib$(LIBNAME).sa
 ifneq ($(wildcard $(STAGE)lib), "")
	$(COPYCMD) $(OBJ_DIR_FINAL)lib$(LIBNAME).a $(LIB_DIR_FINAL)
	-$(COPYCMD) lib$(LIBNAME).sa $(SA_DIR)
 endif
endif
	@for i in $(SUBDIRS); \
	do \
		if test -f $$i/makefile; then \
			cd $$i ;\
			$(MAKE) library ;\
			cd .. ;\
		fi; \
	done

#----------------------------------------------------------------------------
# Build executables from objects
#----------------------------------------------------------------------------
ifeq ($(IS_APP), 1)
binary : banner directories objects $(USR_OBJS) static_data $(USR_SA_OBJS)
else
binary : banner
endif
ifeq ($(IS_APP), 1)
	@echo APP: $(APPNAME)
	$(LINK) $(LFLAGS) -o $(OBJ_DIR_FINAL)$(APPNAME).elf $(USR_OBJS) $(addsuffix .a, $(addprefix $(LIB_DIR_FINAL), $(addprefix lib, $(LIBS))))
	$(OBJCOPY) $(OBJCOPY_FLAGS) "$(OBJ_DIR_FINAL)$(APPNAME).elf" "$(OBJ_DIR_FINAL)$(APPNAME).hex"
	-cat $(USR_SA_OBJS) > $(APPNAME).sa
 ifneq ($(wildcard $(STAGE)app), "")	
	$(COPYCMD) $(OBJ_DIR_FINAL)$(APPNAME).elf $(APP_DIR_FINAL)
	$(COPYCMD) $(OBJ_DIR_FINAL)$(APPNAME).hex $(APP_DIR_FINAL)
	-$(COPYCMD) $(APPNAME).sa $(SA_DIR)
 endif
endif
	@for i in $(SUBDIRS); \
	do \
		if test -f $$i/makefile; then \
			cd $$i ;\
			$(MAKE) binary ;\
			cd .. ;\
		fi; \
	done

#----------------------------------------------------------------------------
# Code to create object directories (only for folders with lib/app targets
#----------------------------------------------------------------------------
directories : $(OBJ_DIR_FINAL) $(APP_DIR_FINAL) $(LIB_DIR_FINAL)
$(OBJ_DIR) :
	@if test -d $(OBJ_DIR); then echo exists ;\
		else \
		mkdir $(OBJ_DIR)	;\
	fi;

$(OBJ_DIR)$(ARCH) : $(OBJ_DIR)
	@if test -d $(OBJ_DIR)$(ARCH); then echo exists; \
		else \
		mkdir $(OBJ_DIR)$(ARCH)    ;\
	fi;

$(OBJ_DIR)$(ARCH)/$(VARIANT) : $(OBJ_DIR)$(ARCH)
	@if test -d $(OBJ_DIR)$(ARCH)/$(VARIANT); then echo exists; \
		else \
		mkdir $(OBJ_DIR)$(ARCH)/$(VARIANT)    ;\
	fi;

$(OBJ_DIR_FINAL) : $(OBJ_DIR)$(ARCH)/$(VARIANT)
	@if test -d $(OBJ_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN); then echo exists; \
		else \
		mkdir $(OBJ_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)    ;\
	fi;

$(APP_DIR) :
	@if test -d $(APP_DIR); then echo exists ;\
		else \
		mkdir $(APP_DIR)	;\
	fi;

$(APP_DIR)$(ARCH) : $(APP_DIR)
	@if test -d $(APP_DIR)$(ARCH); then echo exists; \
		else \
		mkdir $(APP_DIR)$(ARCH)    ;\
	fi;

$(APP_DIR)$(ARCH)/$(VARIANT) : $(APP_DIR)$(ARCH)
	@if test -d $(APP_DIR)$(ARCH)/$(VARIANT); then echo exists; \
		else \
		mkdir $(APP_DIR)$(ARCH)/$(VARIANT)    ;\
	fi;

$(APP_DIR_FINAL) : $(APP_DIR)$(ARCH)/$(VARIANT)
	@if test -d $(APP_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN); then echo exists; \
		else \
		mkdir $(APP_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)    ;\
	fi;

$(LIB_DIR) :
	@if test -d $(LIB_DIR); then echo exists ;\
		else \
		mkdir $(LIB_DIR)	;\
	fi;

$(LIB_DIR)$(ARCH) : $(LIB_DIR)
	@if test -d $(LIB_DIR)$(ARCH); then echo exists; \
		else \
		mkdir $(LIB_DIR)$(ARCH)    ;\
	fi;

$(LIB_DIR)$(ARCH)/$(VARIANT) : $(LIB_DIR)$(ARCH)
	@if test -d $(LIB_DIR)$(ARCH)/$(VARIANT); then echo exists; \
		else \
		mkdir $(LIB_DIR)$(ARCH)/$(VARIANT)    ;\
	fi;

$(LIB_DIR_FINAL) : $(LIB_DIR)$(ARCH)/$(VARIANT)
	@if test -d $(LIB_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN); then echo exists; \
		else \
		mkdir $(LIB_DIR)$(ARCH)/$(VARIANT)/$(TOOLCHAIN)    ;\
	fi;

#----------------------------------------------------------------------------
# Rule to build static analysis data from source
#----------------------------------------------------------------------------
static_data : directories $(C_OBJ) $(CPP_OBJ) $(PORT_C_OBJ) $(PORT_CPP_OBJ)
$(OBJ_DIR_FINAL)%.c.sa : %.c
	-$(CLANG) $(CLANGFLAGS) $< >> $@ 2> $@

$(OBJ_DIR_FINAL)%.cpp.sa : %.cpp
	-$(CLANG) $(CLANGFLAGS) $< >> $@ 2> $@

$(PORT_OBJ_DIR)%.c.sa : $(PORT_DIR)%.c $(PORT_OBJ_DIR)
	-$(CLANG) $(CLANGFLAGS) $< >> $@ 2> $@

$(PORT_OBJ_DIR)%.cpp.sa : $(PORT_DIR)%.cpp $(PORT_OBJ_DIR)
	-$(CLANG) $(CLANGFLAGS) $< >> $@ 2> $@

#----------------------------------------------------------------------------
# Rule to build C code into object files
#----------------------------------------------------------------------------
objects : directories $(C_OBJ) $(CPP_OBJ) $(PORT_C_OBJ) $(PORT_CPP_OBJ)
$(OBJ_DIR_FINAL)%.c.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR_FINAL)%.cpp.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

$(PORT_OBJ_DIR)%.c.o : $(PORT_DIR)%.c $(PORT_OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(PORT_OBJ_DIR)%.cpp.o : $(PORT_DIR)%.cpp $(PORT_OBJ_DIR)
	$(CPP) $(CPPFLAGS) $< -o $@

$(PORT_OBJ_DIR) : $(PORT_DIR)
	@if test -d $(PORT_OBJ_DIR); then echo exists; \
		else \
		mkdir $(PORT_OBJ_DIR)	; \
	fi;

