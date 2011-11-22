#ifndef BOOT_H
#define BOOT_H

#include <ioavr.h>
//#include <limits.h>

/* parts of boot.h from AVR GCC */

//#define _BV(bit) (1 << (bit))
//#define __SPM_REG    SPMCSR
//
///* Create common bit definitions. */
//#define __COMMON_ASB    RWWSB
//#define __COMMON_ASRE   RWWSRE
//
//#define BOOTLOADER_SECTION    __attribute__ ((section (".bootloader")))
//
///** \ingroup avr_boot
//    \def boot_spm_busy()
//    Check if the SPM instruction is busy. */
//
//#define boot_spm_busy()               (__SPM_REG & (unsigned char)_BV(SPMEN))
//
///** \ingroup avr_boot
//    \def boot_spm_busy_wait()
//    Wait while the SPM instruction is busy. */
//
//#define boot_spm_busy_wait()          do{}while(boot_spm_busy())
//
///* Define the bit positions of the Boot Lock Bits. */
//
//#define BLB12           5
//#define BLB11           4
//#define BLB02           3
//#define BLB01           2
//
//#define __BOOT_PAGE_ERASE         (_BV(SPMEN) | _BV(PGERS))
//#define __BOOT_PAGE_WRITE         (_BV(SPMEN) | _BV(PGWRT))
//#define __BOOT_PAGE_FILL          _BV(SPMEN)
//#define __BOOT_RWW_ENABLE         (_BV(SPMEN) | _BV(__COMMON_ASRE))
//#define __BOOT_LOCK_BITS_SET      (_BV(SPMEN) | _BV(BLBSET))
//
//#define __boot_page_fill_normal(address, data)   \
//({                                               \
//    __asm volatile                         \
//    (                                            \
//        "movw  r0, %3\n\t"                       \
//        "movw r30, %2\n\t"                       \
//        "sts %0, %1\n\t"                         \
//        "spm\n\t"                                \
//        "clr  r1\n\t"                            \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "r" ((unsigned char)__BOOT_PAGE_FILL),       \
//          "r" ((unsigned int)address),               \
//          "r" ((unsigned int)data)                   \
//        : "r0", "r30", "r31"                     \
//    );                                           \
//})
//
//#define __boot_page_fill_extended(address, data) \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "movw  r0, %4\n\t"                       \
//        "movw r30, %A3\n\t"                      \
//        "sts %1, %C3\n\t"                        \
//        "sts %0, %2\n\t"                         \
//        "spm\n\t"                                \
//        "clr  r1\n\t"                            \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
//          "r" ((unsigned char)__BOOT_PAGE_FILL),       \
//          "r" ((unsigned long)address),               \
//          "r" ((unsigned int)data)                   \
//        : "r0", "r30", "r31"                     \
//    );                                           \
//})
//
///*
//#define __boot_page_erase_normal(address)        \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "movw r30, %2\n\t"                       \
//        "sts %0, %1\n\t"                         \
//        "spm\n\t"                                \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "r" ((unsigned char)__BOOT_PAGE_ERASE),      \
//          "r" ((unsigned int)address)                \
//        : "r30", "r31"                           \
//    );                                           \
//})
//
//#define __boot_page_erase_extended(address)      \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "movw r30, %A3\n\t"                      \
//        "sts  %1, %C3\n\t"                       \
//        "sts %0, %2\n\t"                         \
//        "spm\n\t"                                \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
//          "r" ((unsigned char)__BOOT_PAGE_ERASE),      \
//          "r" ((unsigned long)address)                \
//        : "r30", "r31"                           \
//    );                                           \
//})
//*/
//#define __boot_page_write_normal(address)        \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "movw r30, %2\n\t"                       \
//        "sts %0, %1\n\t"                         \
//        "spm\n\t"                                \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "r" ((unsigned char)__BOOT_PAGE_WRITE),      \
//          "r" ((unsigned int)address)                \
//        : "r30", "r31"                           \
//    );                                           \
//})
//
//#define __boot_page_write_extended(address)      \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "movw r30, %A3\n\t"                      \
//        "sts %1, %C3\n\t"                        \
//        "sts %0, %2\n\t"                         \
//        "spm\n\t"                                \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
//          "r" ((unsigned char)__BOOT_PAGE_WRITE),      \
//          "r" ((unsigned long)address)                \
//        : "r30", "r31"                           \
//    );                                           \
//})
//
//#define __boot_rww_enable()                      \
//({                                               \
//    __asm volatile                        \
//    (                                            \
//        "sts %0, %1\n\t"                         \
//        "spm\n\t"                                \
//        :                                        \
//        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
//          "r" ((unsigned char)__BOOT_RWW_ENABLE)       \
//    );                                           \
//})
//
//
//#if defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) \
//    || defined(__AVR_ATmega323__)
//#error CPU not supported!
//#elif (FLASHEND > USHRT_MAX)
//
///* Extended: >16 bit address */
//#define boot_page_fill(address, data) __boot_page_fill_extended(address, data)
//#define boot_page_erase(address)      __boot_page_erase_extended(address)
//#define boot_page_write(address)      __boot_page_write_extended(address)
//#define boot_rww_enable()             __boot_rww_enable()
//
//#else
//
///* Normal: 16 bit address */
//#define boot_page_fill(address, data) __boot_page_fill_normal(address, data)
//#define boot_page_erase(address)      __boot_page_erase_normal(address)
//#define boot_page_write(address)      __boot_page_write_normal(address)
//#define boot_rww_enable()             __boot_rww_enable()
//
//#endif

#endif
