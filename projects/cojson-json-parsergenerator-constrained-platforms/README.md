# cojson - a JSON parser/generator for constrained platforms

Uploaded by hutorny on 2015-10-22 13:34:00 (rating 0 out of 5)

## Summary

cojson is a C++ pull-type JSON parser/serializer for constrained platforms, such as bare metal applications on low-end MCUs. It does not use memory allocation and has almost no external dependencies. It is not intrusive – it neither forces nor implies any particular design of the application. Instead it adapts to fit any existing application code. It is tolerant to data type mismatching. When such occurs, parser just skips mismatching data and makes best efforts to continue parsing. The parser is recursive, e.g. nested JSON elements are handled with the recursion. However, this recursion is driven by the structure definition, not by the input data, which prevents stack faults on malformed input data.


cojson is character type neutral – it can work with signed or unsigned character, as well as with standard wide character types: wchar\_t, char16\_t and char32\_t. It is also transparent for UTF8 and properly handles BOM sequence.


 


cojson works against a user-defined structure which specifies hierarchy, data types, and data storage access methods. Thus, when parsing is complete, the data already delivered to the application and no further processing needed. The same structure definition is also used for writing JSON. The structure is defined with a set of templetized function, for example, a PDO object with three members can be defined as simple as:



```

#include "cojson.hpp"
using namespace cojson;
#define NAME(s) static inline constexpr const char* s() noexcept { return #s; }
class Pdo {
    struct Name {
        NAME(u)
        NAME(s)
        NAME(v)
    };
    char s[16];
    short u;
    short v; 
    // JSON structure definition
    static const clas<Pdo>& structure() {
        return
            O<Pdo,
                P<Pdo, Name::s, countof(&Pdo::s), &Pdo::s>,
                P<Pdo, Name::u, decltype(Pdo::u), &Pdo::u>,
                P<Pdo, Name::v, decltype(Pdo::v), &Pdo::v>
    >();
    }
public:
    // reading JSON
    inline bool read(lexer& in) { return structure().read(*this,in); }
    // writing JSON
    inline bool write(ostream& out) { return structure().write(*this,out); }
};
```
In this example, JSON structure is defined in a static function structure(), which uses cojson function O to define a JSON object, and P to define a JSON member. s, u, v – are static methods returning property names defined with the macro NAME. Methods read and write are given for illustration and convenience purposes.

## Links

- [Project overview](http://hutorny.in.ua/projects/cojson-a-json-parser-for-constrained-platforms)
- [cojson tutorial](http://hutorny.in.ua/projects/cojson-tutorial)
- [cojson sources](https://github.com/hutorny/cojson)

## Tags

- General Lib.functions
- #Atmega
- JSON
