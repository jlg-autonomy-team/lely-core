/**@file
 * This file is part of the utilities library; it contains the implementation of
 * the byte order functions.
 *
 * @see lely/util/endian.h
 *
 * @copyright 2013-2022 Lely Industries N.V.
 *
 * @author J. S. Seldenthuis <jseldenthuis@lely.com>
 * @author M. W. Hessel <mhessel@lely.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Disable macro definitions of htobe*() and htole*(). This ensures our
// implementation of these functions is exported.
#define __NO_STRING_INLINES

#include "util.h"
#define LELY_UTIL_ENDIAN_INLINE extern inline
#include <lely/util/endian.h>

#include <assert.h>

static inline void bitcpy(
		uint_least8_t *dst, uint_least8_t src, uint_least8_t mask);

void
bcpybe(uint_least8_t *dst, int dstbit, const uint_least8_t *src, int srcbit,
		size_t n)
{
	if (!n)
		return;

	assert(dst);
	assert(src);

	dst += dstbit / 8;
	dstbit %= 8;
	if (dstbit < 0) {
		dst--;
		dstbit += 8;
	}

	src += srcbit / 8;
	srcbit %= 8;
	if (srcbit < 0) {
		src--;
		srcbit += 8;
	}

	uint_least8_t first = (uint_least8_t)(0xffu >> dstbit);
	uint_least8_t last = ~(0xffu >> (((size_t)dstbit + n) % 8)) & 0xffu;

	int shift = dstbit - srcbit;
	if (shift) {
		int right = shift & (8 - 1);
		int left = -shift & (8 - 1);

		if ((size_t)dstbit + n <= 8) {
			if (last)
				first &= last;
			if (shift > 0) {
				bitcpy(dst, (uint_least8_t)(*src >> right),
						first);
			} else if ((size_t)srcbit + n <= 8) {
				bitcpy(dst, (uint_least8_t)(*src << left),
						first);
			} else {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						*src << left | src[1] >> right),
						first);
				// clang-format on
			}
		} else {
			uint_least8_t b = *src++ & 0xffu;
			if (shift > 0) {
				bitcpy(dst, (uint_least8_t)(b >> right), first);
			} else {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						b << left | *src >> right),
						first);
				// clang-format on
				b = *src++ & 0xffu;
			}
			dst++;
			n -= (size_t)(8 - dstbit);

			n /= 8;
			while (n--) {
				// clang-format off
				*dst++ = (uint_least8_t)(
						b << left | *src >> right)
						& 0xffu;
				// clang-format on
				b = *src++ & 0xffu;
			}

			if (last) {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						b << left | *src >> right),
						last);
				// clang-format on
			}
		}
	} else {
		if ((size_t)dstbit + n <= 8) {
			if (last)
				first &= last;
			bitcpy(dst, *src, first);
		} else {
			if (dstbit > 0) {
				bitcpy(dst++, *src++, first);
				n -= (size_t)(8 - dstbit);
			}

			n /= 8;
			while (n--)
				*dst++ = *src++ & 0xffu;

			if (last)
				bitcpy(dst, *src, last);
		}
	}
}

void
bcpyle(uint_least8_t *dst, int dstbit, const uint_least8_t *src, int srcbit,
		size_t n)
{
	if (!n)
		return;

	assert(dst);
	assert(src);

	dst += dstbit / 8;
	dstbit %= 8;
	if (dstbit < 0) {
		dst--;
		dstbit += 8;
	}

	src += srcbit / 8;
	srcbit %= 8;
	if (srcbit < 0) {
		src--;
		srcbit += 8;
	}

	uint_least8_t first = (0xffu << (size_t)dstbit) & 0xffu;
	uint_least8_t last = ~(0xffu << (((size_t)dstbit + n) % 8)) & 0xffu;

	int shift = dstbit - srcbit;
	if (shift) {
		int right = -shift & (8 - 1);
		int left = shift & (8 - 1);

		if ((size_t)dstbit + n <= 8) {
			if (last)
				first &= last;
			if (shift > 0) {
				bitcpy(dst, (uint_least8_t)(*src << left),
						first);
			} else if ((size_t)srcbit + n <= 8) {
				bitcpy(dst, (uint_least8_t)(*src >> right),
						first);
			} else {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						*src >> right | src[1] << left),
						first);
				// clang-format on
			}
		} else {
			uint_least8_t b = *src++ & 0xffu;
			if (shift > 0) {
				bitcpy(dst, (uint_least8_t)(b << left), first);
			} else {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						b >> right | *src << left),
						first);
				// clang-format on
				b = *src++ & 0xffu;
			}
			dst++;
			n -= (size_t)(8 - dstbit);

			n /= 8;
			while (n--) {
				// clang-format off
				*dst++ = (uint_least8_t)(
						(b >> right | *src << left))
						& 0xffu;
				// clang-format on
				b = *src++ & 0xffu;
			}

			if (last) {
				// clang-format off
				bitcpy(dst, (uint_least8_t)(
						b >> right | *src << left),
						last);
				// clang-format on
			}
		}
	} else {
		if ((size_t)dstbit + n <= 8) {
			if (last)
				first &= last;
			bitcpy(dst, *src, first);
		} else {
			if (dstbit > 0) {
				bitcpy(dst++, *src++, first);
				n -= (size_t)(8 - dstbit);
			}

			n /= 8;
			while (n--)
				*dst++ = *src++ & 0xffu;

			if (last)
				bitcpy(dst, *src, last);
		}
	}
}

static inline void
bitcpy(uint_least8_t *dst, uint_least8_t src, uint_least8_t mask)
{
	*dst = (((src ^ *dst) & mask) ^ *dst) & 0xffu;
}
