# Single Letter Reader

Uploaded by sunge on 2009-02-03 19:55:00 (rating 0 out of 5)

## Summary

With this toy, you can Write Your Message To Friends


In reader.c, there are two functions to show message in display:


/* setting time=100*N(ms) for displaying one character */  

#define DELAY\_CHAR 5  

#define DELAY\_DOT 3  

/*put one character to 16-segment display */  

void led\_put\_char(unsigned char)  

/* put a string to display */  

void led\_put\_str(const char *)


you can write messages to display like the following usage:


led\_put\_char('5');  

led\_put\_str(PSTR("hello, world!")); 


Have fun!

## Links

- [Full resources of Single Letter Reader](http://www.geektalks.cn/filemgmt/visit.php?lid=2)
- [homepage](http://www.geektalks.cn/article.php?story=20080530093318605)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
