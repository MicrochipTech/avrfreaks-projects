/*
    avr-x86 - x86 emulator running on AVR Arduino!
    Copyright (C) 2021 @raspiduino

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

/*
    Note on avr-x86's RAM:
    A few KB is not enough for our emulator. So ram.cpp will create a
    virutal RAM on the SD card, read and write from it and send to the emulator.

    Virtual RAM file is located in RAM_FILE macro path
    Default is 'RAM.BIN'
*/

#include "ram.h"

SdCard card;
Fat16 file;

#ifdef BOOT_PROMPT
char filepath[12]; // Virtual disk filepath
#endif
#ifndef BOOT_PROMPT
char filepath[] = BOOT_FILE;
#endif

void sdinit(){
    /* Init the SD card */
    Serial.print(F("Init SD card"));
    while(!card.begin(CHIP_SELECT)) Serial.print("."); // Mount filesystem
    Serial.print(F(". Done! Init filesystem"));
    while(!Fat16::init(&card)) Serial.print(".");
    Serial.println(F(". Done!"));
}

void ramload(){
    #ifdef BOOT_PROMPT
    fileinput();
    while(!file.open(filepath, O_READ|O_WRITE)){
        //Serial.print(F("Error opening "));
        //Serial.println(filepath);
        fileinput();
    }
    #endif

    #ifndef ONE_USE
    int ramloc = 0;
    char buf[RAM_BUFFER];
    Serial.print(F("Loading disk image"));
    
    while (1) {
        // Read disk image
        file.open(filepath, O_READ);
        file.seekSet(ramloc); // Change the cusor
        int nr = file.read(buf, RAM_BUFFER); // Read disk image
        //const int ntb = file.fileSize()%RAM_BUFFER; // Number left on the last read
        //if (nr <= ntb) break; // Break at the end of file
        file.close();

        // Write to vram
        file.open(RAM_FILE, O_WRITE|O_CREAT); // Open ram image file
        file.seekSet(ramloc); // Change the cusor
        file.write(buf, RAM_BUFFER);
        file.close();

        if (nr < RAM_BUFFER) break;
        ramloc = ramloc + RAM_BUFFER; // Increase ramloc

        //file.open(filepath, O_READ);
        Serial.print(F("."));
    }

    Serial.println(F(". Done!"));
    #endif
}

byte ramread(uint32_t addr){
    byte buf[1];

    #ifdef ONE_USE
    file.open(filepath, O_READ); // Open image file
    #endif
    #ifndef ONE_USE
    file.open(RAM_FILE, O_READ); // Open ram image file
    #endif

    file.seekSet(addr); // Seek to the location
    file.read(buf, 1);
    file.close();
    return buf[0]; // Return the buffer
}

void ramwrite(uint32_t addr, byte value){
    byte buf[1];
    buf[0] = value;

    #ifdef ONE_USE
    file.open(filepath, O_WRITE);
    #endif
    #ifndef ONE_USE
    file.open(RAM_FILE, O_WRITE);
    #endif

    file.seekSet(addr);
    file.write(buf, 1);
    file.close();
}

/* fileinput() - Ask for virtual disk file to boot from */
#ifdef BOOT_PROMPT
void fileinput(){
    Serial.print(F("$ "));
    
    //while(Serial.available() == 0); // Wait until Serial input
    
    String tmpstring;
    while (1){
        if(Serial.available() > 0){
            // Read the char
            int incomingByte = Serial.read(); //Read the character ASCII number to the incomingByte varrible
            
            if(incomingByte == 10 | incomingByte == 13) break;
            else{
                tmpstring = String(tmpstring + char(incomingByte));
            }
        }
    }

    tmpstring.toUpperCase(); // Convert the string to upper case for FAT filesystem
    tmpstring.toCharArray(filepath, tmpstring.length()+1); // Convert the filepath to char array
    Serial.println(filepath);
}
#endif
