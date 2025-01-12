 /** 
 **************************************************************
 * @file mylib/s4743556_hamming.c
 * @author Adnaan Buksh - 47435568
 * @date 20032024
 * @brief Hamming byte encode, byte decode and half byte encode functions
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s474355_lib_hamming_byte_encode(unsigned char value) - Encodes a byte using Hamming code
 * s474355_lib_hamming_byte_decode(unsigned short value) - Decodes a byte using Hamming code
 * s474355_lib_hamming_parity_error(unsigned char value) - Checks for parity errors in a byte
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

extern unsigned short s4743556_lib_hamming_byte_encode(unsigned char);
extern unsigned char s4743556_lib_hamming_byte_decode(unsigned short);
extern unsigned char s4743556_lib_hamming_parity_error(unsigned char);

