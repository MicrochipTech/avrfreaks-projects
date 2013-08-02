
// TCXO SPI Real Time Clock library for Dallas/Maxim DS3234 (SPI MODE 3)
// Klaus Varis (klaus.varis@wippies.fi)
// ver. 130801

#include <bcd.h>
#include <spi.h>
#define CS_ENABLE PORTC.2 = 0;
#define CS_DISABLE PORTC.2 = 1;

const char NONE = 0;
const char A1   = 1;
const char A2   = 2; // Alarm resolution one minute, second argument bypassed therefore
const char ALL  = 3; // Apply to both alarms on particular function 

char DS3234_initialize();
void DS3234_set_time(char hours, char minutes, char seconds);
void DS3234_get_time(char *hours, char *minutes, char *seconds);
void DS3234_set_date(char years, char months, char date, char day);
void DS3234_get_date(char *years, char *months, char *date, char *day);
void DS3234_set_alarm(char A1A2, char date, char hours, char minutes, char seconds);
void DS3234_get_alarm(char A1A2, char *date, char *hours, char *minutes, char *seconds);
void DS3234_enable_alarms(char A1A2); // Changes are not incremental.
void DS3234_disable_alarms();
void DS3234_clear_alarms();
char DS3234_read_alarm_flags();
char DS3234_get_aging_offset();
void DS3234_set_aging_offset(char aging_value);
float DS3234_temperature();
unsigned char DS3234_SRAM_read(unsigned char address);
unsigned char DS3234_SRAM_write(unsigned char address, unsigned char data);

const char * flash day_of_the_week[8]= {"Error", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char control_register = 0b00000100; // 0Eh: Enable batt. oscillator, no square-wave OP, no no interrupts (Dallas Semiconductor/Maxim datasheet paging_value 14, rev 3)
const char status_register = 0b00000000; // 0Fh: Read OSF & reset, no batt. 32kHz, TCXO 64 sample rate, no VCC 32kHz (Dallas Semiconductor/Maxim datasheet paging_value 15, rev 3)

char DS3234_initialize()
{
    unsigned char status, temp;
    unsigned int cnt;
    do
    {
        CS_ENABLE;
        spi(0x0F);
        temp = spi(0x00);        
        temp = temp & 0b00000100; // Check busy flag
        CS_DISABLE;
        cnt++;        
    }
    while ((temp != 0) && (cnt < 0xFFFE));
    if (cnt == 0xFFFE) return -1;                             
    CS_ENABLE;
    spi(0x0F);
    temp = spi(0x00);
    CS_DISABLE;
    status = temp >> 7; // Check Oscillator Stop Flag (OSF) from control/status register                    
    CS_ENABLE;            
    spi(0x98);
    spi(0xAA);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;            
    spi(0x18);
    temp = spi(0x00);
    CS_DISABLE;
    if (0xAA != temp) return -2; // Check connection integrity or chip exist
    CS_ENABLE;            
    spi(0x0E);
    temp = spi(0x00);
    CS_DISABLE;
    if (temp != control_register)
    {
        CS_ENABLE;            
        spi(0x8E);
        temp = spi(control_register); // Update the control register with desired flags
        CS_DISABLE;
        #asm("nop");        
        CS_ENABLE;                            
        spi(0x0E);
        temp = spi(0x00);
        CS_DISABLE;
        if (temp != control_register) return -3;  // Verify updates               
    }
    CS_ENABLE;            
    spi(0x0F);
    temp = spi(0x00);
    CS_DISABLE;
    if ((temp & 0b11111100) != status_register) // Preserve alarm flags if other bits are as they should. Two LSB bits can written only to zero.
    {  
        CS_ENABLE;
        spi(0x8F);                     
        spi(status_register); // Update the status register with desired flags
        CS_DISABLE;
        #asm("nop");                
        CS_ENABLE;
        spi(0x0F);                     
        temp = spi(status_register);         
        CS_DISABLE;                
        if (temp != status_register) return -4; // Verify updates              
    }            
    if (status == 1) return 0;
    else return 1;
}

void DS3234_set_time(char hours, char minutes, char seconds)
{
    CS_ENABLE;
    spi(0x80);
    spi(bin2bcd(seconds));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x81);
    spi(bin2bcd(minutes));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x82);
    spi(bin2bcd(hours));
    CS_DISABLE;
}

void DS3234_get_time(char *hours, char *minutes, char *seconds)
{
    CS_ENABLE;
    spi(0x00);
    *seconds = bcd2bin(spi(0x00));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x01);
    *minutes = bcd2bin(spi(0x00));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x02);
    *hours = bcd2bin(spi(0x00));
    CS_DISABLE;    
}

void DS3234_set_date(char years, char months, char date, char day)
{
    CS_ENABLE;
    spi(0x83);
    spi(bin2bcd(day));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x84);
    spi(bin2bcd(date));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x85);
    spi(bin2bcd(months));
    CS_DISABLE;
    #asm("nop");    
    CS_ENABLE;
    spi(0x86);
    spi(bin2bcd(years));
    CS_DISABLE;
}

void DS3234_get_date(char *years, char *months, char *date, char *day)
{
    char temp;
    CS_ENABLE;
    spi(0x03);
    temp = spi(0x00);
    *day = bcd2bin(temp);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x04);
    temp = spi(0x00);
    *date = bcd2bin(temp);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x05);
    temp = spi(0x00);
    *months = bcd2bin(temp);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x06);
    temp = spi(0x00);
    *years = bcd2bin(temp);
    CS_DISABLE;
}

void DS3234_set_alarm(char A1A2, char date, char hours, char minutes, char seconds)
{
    if (A1A2 == A1)
    {
        CS_ENABLE;
        spi(0x87);
        spi(bin2bcd(seconds));
        CS_DISABLE;
        #asm("nop");
        CS_ENABLE;
        spi(0x88);
        spi(bin2bcd(minutes));
        CS_DISABLE;
        #asm("nop");
        CS_ENABLE;
        spi(0x89);
        spi(bin2bcd(hours));
        CS_DISABLE;      
        #asm("nop");
        CS_ENABLE;
        spi(0x8A);
        spi(bin2bcd(date));
        CS_DISABLE;   
    }     
    if (A1A2 == A2)
    {    
        CS_ENABLE;
        spi(0x8B);
        spi(bin2bcd(minutes));
        CS_DISABLE;
        #asm("nop");
        CS_ENABLE;
        spi(0x8C);
        spi(bin2bcd(hours));
        CS_DISABLE;      
        #asm("nop");
        CS_ENABLE;
        spi(0x8D);
        spi(bin2bcd(date));
        CS_DISABLE;
    }   
}

void DS3234_get_alarm(char A1A2, char *date, char *hours, char *minutes, char *seconds)
{
    unsigned char temp;
    if (A1A2 == A1)
    {
        CS_ENABLE;
        spi(0x07);
        temp = spi(0x00);
        *seconds = bcd2bin(temp);
        CS_DISABLE;
        #asm("nop");    
        spi(0x08);
        temp = spi(0x00);
        *minutes = bcd2bin(temp);
        CS_DISABLE;
        #asm("nop");
        CS_ENABLE;
        spi(0x09);
        temp = spi(0x00);
        *hours = bcd2bin(temp);
        CS_DISABLE;      
        #asm("nop");
        CS_ENABLE;
        spi(0x0A);
        temp = spi(0x00);
        *date = bcd2bin(temp);
        CS_DISABLE;
    }
    if (A1A2 == A2)
    {
        CS_ENABLE;
        *seconds = 0;
        spi(0x0B);
        temp = spi(0x00);
        *minutes = bcd2bin(temp);
        CS_DISABLE;
        #asm("nop");
        CS_ENABLE;
        spi(0x0C);
        temp = spi(0x00);
        *hours = bcd2bin(temp);
        CS_DISABLE;      
        #asm("nop");
        CS_ENABLE;
        spi(0x0D);
        temp = spi(0x00);
        *date = bcd2bin(temp);
        CS_DISABLE;
    }          
}

void DS3234_enable_alarms(char A1A2)
{
    unsigned char temp;
    CS_ENABLE;
    spi(0x0E);
    temp = spi(0x00);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x8E);
    spi((temp & 0b11111100) | (A1A2 & 3));
    CS_DISABLE;
}

void DS3234_disable_alarms()
{
    DS3234_enable_alarms(NONE);
    DS3234_set_alarm(A1, 0x1F, 0x1F, 0x3F, 0x3F );  // Trying to circumvent false DS3234_read_alarm_flags() checks on int-FET powered systems.
    DS3234_set_alarm(A2, 0x1F, 0x1F, 0x3F, 0);      // Second value is not used on A2. DS3234_read_alarm_flags is unable to utilize INTCN, A2IE or A1IE bits as they are set to default values when power is first applied.    
}

void DS3234_clear_alarms()
{
    unsigned char temp;
    CS_ENABLE;
    spi(0x0F);
    temp = spi(0x00);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x8F);
    spi(temp & 0b11111100);
    CS_DISABLE;
}

char DS3234_read_alarm_flags()
{
    unsigned char temp;         
    CS_ENABLE;
    spi(0x0F);
    temp = spi(0x00) & 0b00000011;  // AIE-bits on register 0Eh will affect only to the interrupt generation. 
    CS_DISABLE;                     // Alarm flags will be updated even when AIE-bits are cleared.
    return temp;
}

char DS3234_get_aging_offset()
{
    char temp;
    CS_ENABLE;
    spi(0x10);
    temp = spi(0x00);    
    CS_DISABLE;
    return temp;    
}

void DS3234_set_aging_offset(char aging_value)
{
    // Positive aging values add capacitance to the array thus slowing the oscillator frequency
    CS_ENABLE;
    spi(0x90);
    spi(aging_value);    
    CS_DISABLE;
}

float DS3234_temperature()
{
    float temp;
    unsigned char byte[2];
    CS_ENABLE;
    spi(0x11);
    byte[0] = bcd2bin(spi(0x00));
    CS_DISABLE;  
    #asm("nop");
    CS_ENABLE;
    spi(0x12);
    byte[1] = bcd2bin(spi(0x00) >> 6);
    CS_DISABLE;         
    if (byte[0] >= 0b10000000) 
    {         
        return 0; // It seems that commercial (non-industrial) version of the chip does not display temperature values below zero
        /*
        byte[0] = ~byte[0]+1;
        temp = 0 - (byte[0] + (0.25 * byte[1]));
        return temp;
        */
    }
    else temp = byte[0] + (0.25 * byte[1]);    
    return temp;
}

unsigned char DS3234_SRAM_read(unsigned char address)
{
    unsigned char temp;
    CS_ENABLE;
    spi(0x98);
    spi(address);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x19);
    temp = spi(0x00);
    CS_DISABLE;
    return temp;    
}

unsigned char DS3234_SRAM_write(unsigned char address, unsigned char data)
{
    unsigned char temp;
    CS_ENABLE;
    spi(0x98);
    spi(address);
    CS_DISABLE;
    #asm("nop");
    CS_ENABLE;
    spi(0x99);
    temp = spi(data);
    CS_DISABLE;
    return temp;  
}