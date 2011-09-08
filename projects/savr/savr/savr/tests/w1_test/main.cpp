/*************************************************************//**
 * @file main.c
 *
 * @author Stefan Filipek
 ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <savr/cpp_pgmspace.h>
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/twi.h>
#include <savr/w1.h>
#include <savr/dstherm.h>

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()


static W1 *wire = NULL;

static bool ParseAddress(W1::Address &address, char *text) {
    uint8_t i=8;
    char temp[3];
    temp[2] = 0;

    while(i-->0 && *text && *(text+1)) {
        temp[0] = *text++;
        temp[1] = *text++;
        address.array[i] = strtoul(temp, NULL, 16);
    }
    if(i != 0xff) {
        return false;
    }
    printf_P(PSTR("Address: "));
    W1::PrintAddress(address);
    putchar('\n');
    return true;
}


uint8_t wrap_W1_Search(char *args) {

    printf_P(PSTR("Devices found:\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->SearchROM(address, token)) {
        W1::PrintAddress(address);
        putchar('\n');
    }


    return 0;
}


uint8_t wrap_W1_Alarm(char *args) {

    printf_P(PSTR("Devices found (Alarm):\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->AlarmSearch(address, token)) {
        W1::PrintAddress(address);
        putchar('\n');
    }


    return 0;
}

uint8_t wrap_W1_MatchROM(char *args) {
    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    wire->MatchROM(address);
    return 0;
}

uint8_t wrap_W1_Reset(char *args) {
    if(!wire->Reset()) {
        printf_P(PSTR("No "));
    }
    printf_P(PSTR("Presence!\n"));

    return 0;
}

uint8_t wrap_W1_ReadByte(char *args) {
    char *token;
    char *currentArg;

    uint8_t b = 1;
    currentArg = strtok_r(args, " ", &token);
    if(currentArg != NULL) {
        b = strtoul(currentArg, (char**) NULL, 0);
    }

    printf_P(PSTR("Reading %d bytes:"), b);
    while(b --> 0) {
        printf(" 0x%02X", wire->ReadByte());
    }
    putchar('\n');
    return 0;
}

uint8_t wrap_W1_WriteByte(char *args) {
    char *token;
    char *currentArg;

    uint8_t b;
    currentArg = strtok_r(args, " ", &token);


    while(currentArg != NULL) {
        b = strtoul(currentArg, (char**) NULL, 0);
        currentArg = strtok_r(NULL, " ", &token);

        printf_P(PSTR("Sending: 0x%02X\n"), b);
        wire->WriteByte(b);
    }
    return 0;
}


uint8_t
wrap_GetTemp(char *args)
{
    double dtemp;

    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    DSTherm therm(*wire, address);

    therm.StartConversion();

    dtemp = therm.GetTemp(false);
    printf_P(PSTR("  C Temp: %f\n"), dtemp);

    dtemp = 1.8*dtemp + 32;
    printf_P(PSTR("  F Temp: %f\n"), dtemp);

    return 0;
}

uint8_t wrap_PollTemp(char *args) {
    double dtemp;

    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }

    DSTherm therm(*wire, address);
    while(1) {
        therm.StartConversion();
        dtemp = therm.GetTemp(true);
        printf_P(PSTR("Temp: %f F\n"), dtemp);
    }
}


uint8_t wrap_GetAll(char *args) {
    const size_t MAX_COUNT = 5000;
    size_t count = 0;
    double dtemp;


    if(!wire->Reset()) {
        printf_P(PSTR("No presence detected.\n"));
        return 1;
    }

    // Make all devices start a conversion
    wire->WriteByte(0xCC);
    wire->WriteByte(0x44);


    while(wire->ReadByte() == 0x00) {
        if(count++ > MAX_COUNT) {
            printf_P(PSTR("Device took too long to perform measurement.\n"));
            return 1;
        }
    }

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->SearchROM(address, token)) {
        W1::PrintAddress(address);
        DSTherm therm(*wire, address);
        dtemp = therm.GetTemp(true);
        printf_P(PSTR(": %f F\n"), dtemp);
    }
    return 0;
}


uint8_t wrap_PollAll(char *args) {
    while(1) {
        wrap_GetAll(args);
        putchar('\n');
    }
    return 0;
}


/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmdList = {
    {"Reset",           wrap_W1_Reset,          "Resets the 1-Wire bus and looks for presence."},
    {"Search",          wrap_W1_Search,         "Scans the bus and prints any addresses found"},
    {"Alarm",           wrap_W1_Alarm,          "Scans the bus and prints any addresses found using AlarmSearch"},
    {"Match",           wrap_W1_MatchROM,       "Select a device using MatchROM (Select <address>)"},
    {"Read",            wrap_W1_ReadByte,       "Read a byte from the bus (Read [num bytes])"},
    {"Write",           wrap_W1_WriteByte,      "Write at least one byte to the bus (Write <byte> [byte] ...)"},
    {"GetTemp",         wrap_GetTemp,           "Setup a temp conversion and read the result (GetTemp <address>)"},
    {"PollTemp",        wrap_PollTemp,          "Continually perform temp conversion -- never returns (PollTemp <address>)"},
    {"GetAll",          wrap_GetAll,            "Get temps from all devices once"},
    {"PollAll",         wrap_PollAll,           "Continually get temps from all devices -- never returns"},

};
static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("\n1-Wire Test\n")
#define promptString   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::Init(F_CPU, 38400);  // bps

    W1 localWire(F_CPU, GPIO::C0);
    wire = &localWire;

    Term::Init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::Run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
