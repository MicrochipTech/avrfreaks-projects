/*
    INTSQRT.C

    Given a uint32 number, n, it calculates m, the INTEGER square root of n, and
    the "remainder", k, such that m^2 + k = n for the largest non-negative m and
    smallest non-negative k.

    Note:
    If you want to ignore the "remainder" and get the CLOSEST integer square
    root, m, where ABS((m^2)-n) is the smallest possible value, then:

    If the "remainder" calculated by this program is strictly larger than the
    calculated square root, then add 1 to the square root.

    SO:
    if ("remainder" <= square root) leave square root as is.
    if ("remainder > square root) increment the square root by 1.
 */

#include <stdlib.h>
#include <stdio.h>

#include "inttypes.h"


typedef struct {
    uint32 square;
    uint32 remainder;
} calcReg_t;

union uShift {
    calcReg_t calcReg;
    uint64    shifter;
} shift;

//uint32 origSquare;
//uint32 squareRoot;
uint32 testValue;

uint   bitCount;

#define SHIFTER   shift.shifter
#define REMAINDER shift.calcReg.remainder
#define SQUARE    shift.calcReg.square



void sqrt2(long* origSquare, long* squareRoot)
{

    // Initialise some variables.
    // MAXLBITS is a multiple of 8, so it is always even.
    bitCount = MAXLBITS;
    // Put the original input square into the shift calculator.
    SQUARE = (uint64)*origSquare;
    // Set the remainder to 0;
    REMAINDER = 0ul;
    // Set the square root to 0;
    *squareRoot = 0ul;

    // Find the MS bit-pair.
    while ( (bitCount) && (0ul == REMAINDER) ) {
        SHIFTER <<= 2ull;
        bitCount -= 2;
    }

    // MS bit of square root is always 1 (unless original square was 0).
    if (REMAINDER) {
        *squareRoot = 1ul;
        REMAINDER -= 1ul;
    }

    // Calculate the square root and remainder.
    while ( bitCount ) {

        // Get next bit-pair from SQUARE into REMAINDER, and decrement bitCount.
        SHIFTER <<= 2ull;
        bitCount -= 2;

        // Set testValue = squareRoot*4 + 1.
        testValue = *squareRoot << 2ul;
        ++testValue;

        // Add new LS bit to square root.  Let it be 0 for now.
        *squareRoot <<= 1ul;

        // If testValue can be subtracted from remainder, increment squareRoot
        // and subtract testValue.
        if ( testValue <= REMAINDER ) {
            (*squareRoot)++;
            REMAINDER -= testValue;
        }
    }

}
