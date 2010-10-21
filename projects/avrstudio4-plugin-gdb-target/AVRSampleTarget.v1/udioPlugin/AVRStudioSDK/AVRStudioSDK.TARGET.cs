using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    public static class TARGET
    {
        public enum OPTIONS
        {
            OPTIONS_FIRSTINIT = 1,  // 1) Loads settings from the project settings XML file
                                    // 2) Configures the target simulator/emulator with the loaded settings.
            OPTIONS_WIZARD = 2,     // 1) Loads settings from the project settings XML file
                                    // 2) Displays the options dialog internally to allow the user to alter the settings.
                                    // 3) Configures the target simulator/emulator with the new settings.
            OPTIONS_SHOWDIALOG = 3, // The options dialog are displayed by the client
            OPTIONS_LOADSETTINGS = 4, // This causes the target to load settings from the project settings XML file.
                                        // If the client wants to let the user modify any settings before continuing, 
                                        // it displays the options dialog (or rather, the target property pages) in its own property page site.
            OPTIONS_INITIALIZE = 5  // This causes the target to configure the target simulator/emulator with the new settings.
        }

        public enum MEM
        {
            TARGET_MEM_PROGMEMORY = 0x0001,	// Program/flash	
            TARGET_MEM_SRAM = 0x0002,		// Data
            TARGET_MEM_EEPROM = 0x0004,		// Eeprom
            TARGET_MEM_IO = 0x0008,		// IO
            TARGET_MEM_REGISTERS = 0x0010,		// Register block
            TARGET_MEM_TRACEBUFFER = 0x0020,		// Tracebuffer
            TARGET_MEM_FUSES = 0x0040,		// Fuses
            TARGET_MEM_LOCKBITS = 0x0080,		// Lockbits
            TARGET_MEM_XRAM = 0x0100,		// External RAM
            TARGET_MEM_IO_SHADOW = 0x0200,		// Shadow IO (RO)
            TARGET_MEM_SPMTEMP = 0x0400,		// SIS: SPM Temp Register
            TARGET_MEM_STATEMENTS = 0x0800,		// SIS: Statement breakpoints - statement(0x01) - funccal/start(0x02) - funcret(0x04)
            TARGET_MEM_SRAM_SHADOW = 0x1000,		// General shadow memory area (shadow IO is mapped into this)
            TARGET_MEM_TRACE = 0x2000,		// Trace memory, directly read from target (not cached in ICE50) for debugging

            TARGET_MEM_CUSTOM = 0x8000		// If this bit is set, the memory type is custom	
        }

        public enum STATE
        {
            TARGET_STATE_STOP = 0x01,   //Target is in stop mode
            TARGET_STATE_RUNNING = 0x02, //Target is in run mode
            TARGET_STATE_SLEEP = 0x04, //Target is in sleep mode 
            TARGET_STATE_BUSY = 0x08//Target is busy, e.g. configuring. 
        }
    }
}
