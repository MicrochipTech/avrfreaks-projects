# EEPROM Wear Leveling library

Uploaded by chayanforyou on 2017-07-07 07:01:00 (rating 5 out of 5)

## Summary


```

**WearLeveling:**

The technique I am using is to prefix the data with a 4-byte rolling sequence number where the largest number represents the lastest / current value.
In the case of storing 2 bytes of actual data that would give 6 (4-for sequence & 2-for data) bytes total and then I form into a circular queue arrangement so for 1024 bytes of EEPROM (if your EEPROM size is small or max you have to change the value from library file) it would contain 170 entries and increase endurance 170 times for one Segment.

Then when booting the largest sequence number can be used to determine both the next sequence number to be used and the current tail of the queue.
The following C-code demonstrates, this assumes that upon initial programming the EEPROM area has been erased to values of 0xFF so I ignore a sequence number of 0xFFFFFFFF (as 4-byte sequence no):

By default the MAX\_EEPROM\_SEGMENTS define is set to 2 this means you have two segments available, 0 or 1.
This allows you to define an area of EEPROM for one structure and a secondary area of EEPROM for another.
If you need more segments you have to change it.

**HOW TO USE:**

I designed this library to address a couple of issues when using EEPROM for data storage:

Data corruption is one of this big issues with EEPROM.  From what I've read, the most common reason is a power issue where power drops out and/or brown out detection is not properly configured.
The big problem with data corruption is that when the data is read back from the EEPROM, it may be corrupt and the uC will use corrupt data with can lead to all sorts of problems.
The second issue is that the EEPROM design limit of 100,000 write cycles. If you are trying to save values periodically, this will be a concern if you are writing the data to the same spot of EEPROM each time.
The Library solves both of these problems by using these techniques:

**It does all this with three functions:**

void loadEeprom(uint8\_t ASegment)
uint16\_t getValue(uint8\_t ASegment)
void writeValue(uint8\_t ASegment, uint16\_t value)

The first IMPORTANT thing I want to mention is the ORDER these commands must be used.

At first you must call loadEeprom(uint8\_t ASegment). After that you can call getValue(uint8\_t ASegment) & writeValue(uint8\_t ASegment, uint16\_t value) as many times as you like.
The reason for this order is that Save requires variables that Load set to work properly.
This shouldn't be a problem because the general flow of usage for these commands should follow this order:

**For WinAVR/AVR Studio:**

int main(void)
{
loadEeprom(0);
loadEeprom(1);

printf(getValue(0));
printf(getValue(1));

while (1)
{
writeValue(0, 2);

writeValue(1, 3);
  }
}

......................................

**For Arduino:**
void setup() {
Serial.begin(115200);

loadEeprom(0);
loadEeprom(1);

Serial.println(getValue(0));
Serial.println(getValue(1));

}

void loop() {
writeValue(0, 5);
writeValue(1, 6);

delay(200);

}

Make sure you don't just call save to save the same data as this is wasteful on EEPROM design life.
Keep a flag that indicates when the data has changed and also a flag indicating how long it has been since that last save.
Only save if the data has changed AND it has been a long enough period of time.
```

## Links

- [Wear Leveling library](https://github.com/chayanforyou/WearLeveling)

## Compilers

- Atmel Studio 7

## Tags

- General Lib.functions
- Atmel Studio 7
- EEPROM
