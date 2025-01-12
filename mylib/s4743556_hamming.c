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

#include <stdint.h>
#include <stm32f429xx.h>
#include "s4743556_hamming.h"
#include "s4743556_mfs_led.h"
#include "debug_log.h"

unsigned char s4743556_lib_hamming_parity_error(unsigned char value);

/**
 * Decoded half byte from a byte. Errors must be detected and corrected.
 */
unsigned char s4743556_lib_hamming_byte_decode(unsigned short value) {
    uint8_t d0, d1, d2, d3, h0, h1, h2, s2, s1, s0;
    uint8_t decodedByte = 0x00;
    unsigned char input = value;

    // Extract Hamming bits and Data bits.
    h0 = !!(value & (1 << 1));
    h1 = !!(value & (1 << 2));
    h2 = !!(value & (1 << 3));
    d0 = !!(value & (1 << 4));
    d1 = !!(value & (1 << 5));
    d2 = !!(value & (1 << 6));
    d3 = !!(value & (1 << 7));

    // Calculate the Syndrome.
    s0 = h0 ^ d1 ^ d2 ^ d3;
    s1 = h1 ^ d0 ^ d2 ^ d3;
    s2 = h2 ^ d0 ^ d1 ^ d3;

    if (!(!s0 && !s1 && !s2)) {
      if (!s0 && s1 && s2) {
        d0 = !d0;
      } else if (s0 && !s1 && s2) {
        d1 = !d1;
      } else if (s0 && s1 && !s2) {
        d2 = !d2;
      } else if (s0 && s1 && s2) {
        d3 = !d3;
      } else if (s0 && !s1 && !s2) {
        h0 = !h0;
      } else if (!s0 && s1 && !s2) {
        h1 = !h1;
      } else if (!s0 && !s1 && s2) {
        h2 = !h2;
      }
    }
    decodedByte = (d0 << 0) | (d1 << 1) | (d2 << 2) | (d3 << 3);
    return decodedByte;
}

/**
 * Internal function to Encode a half byte
 * 
 * Implement Hamming Code + parity checking
 * Hamming code is based on the following generator and parity check matrices
 * G = [ 0 1 1 | 1 0 0 0 ;
 *       1 0 1 | 0 1 0 0 ;
 *       1 1 0 | 0 0 1 0 ;
 *       1 1 1 | 0 0 0 1 ;
 *
 * hence H =
 * [ 1 0 0 | 0 1 1 1 ;
 *   0 1 0 | 1 0 1 1 ;
 *   0 0 1 | 1 1 0 1 ];
 *
 * y = x * G, syn = H * y'
 *
 *
 * NOTE: !! is used to get 1 out of non zeros
 */
unsigned char s4743556_lib_hamming_hbyte_encode(unsigned char value) {
	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(value & 0x1);
	d1 = !!(value & 0x2);
	d2 = !!(value & 0x4);
	d3 = !!(value & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
    (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z<8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

	return(out);

}

/**
 * Return the 16bit encoded value of a byte
 * Implement Hamming Code on a full byte of input
 * This means that 16-bits out output is needed
 */
unsigned short s4743556_lib_hamming_byte_encode(unsigned char value) {
	unsigned short out;

	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	out = s4743556_lib_hamming_hbyte_encode(value & 0x0F) |
		((s4743556_lib_hamming_hbyte_encode((value & 0xF0) >> 4)) << 8);

	return(out);

}

/**
 * Return 1 if a parity error has occurred, else 0
*/
unsigned char s4743556_lib_hamming_parity_error(unsigned char value) {
  uint8_t p0 = (value & (1 << 0));
  uint8_t z;
  uint8_t parity = 0;

  for (z = 1; z < 8; z++) {
    parity = parity ^ !!(value & (1 << z));
  }

  if (parity != p0) {
	return 1;
  } else {
	return 0;
  }
}

