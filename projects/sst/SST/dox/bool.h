/** \file bool.h
 *  \brief A user supplied file to define \c bool type.
 *
 * Because there are compilers without standard C99 stdbool.h implementatuon
 * it is needed to provide some intermediate file to allow SST to use bool type.
 * <br>
 * The simplest content of this file can be <code>\#include \<stdbool.h\></code>
 * <br>
 * The SST implementation uses three elements of boolean facility:
 *      - \c bool type
 *      - \c true constant
 *      - \c false constant
 */

