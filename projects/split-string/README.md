# Split string

Uploaded by chandrap on 2009-10-19 08:43:00 (rating 0 out of 5)

## Summary

//string=string to parse, storage=store part of string, part=which part


void save\_string\_part(char* string, char* storage, uint8\_t part){  

 while(*string >= ' ' && part){ //catch cr/lf/etc  

 if(*string++ == ',') part--; //find ',' 'part' times  

 }  

 while(*string >= ' ' && *string != ','){ //until cr/lf/etc or ','  

 *storage++ = *string++; //save  

 }  

 *storage = 0; //0 terminate string  

}

## Compilers

- AtmanAvr C/C++ IDE

## Tags

- General Lib.functions
- AtmanAvr C/C++ IDE
