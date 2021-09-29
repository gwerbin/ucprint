/* UCPrint: Print a sequence of Unicode code points.
 *
 * Original algorithm:
 *   <https://stackoverflow.com/a/4609989/2954547>
 *
 * ISC License:
 *   Copyright 2021 Greg Werbin
 *
 *   Permission to use, copy, modify, and/or distribute this software for any
 *   purpose with or without fee is hereby granted, provided that the above
 *   copyright notice and this permission notice appear in all copies.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 *   IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

enum retcode {
	RET_OK = 0,
	RET_USAGE = 1,
	RET_INVALID = 2,
	RET_MALLOC = 9
};

int print_unicode(unsigned long codepoint) {
	// Buffer for the Unicode string, to stay at 0.
	char *output_buffer;

	// "Cursor" in the output buffer, to be incremented.
	char *b;

	if (codepoint < 0x80) {
		output_buffer = malloc(1 * sizeof(char));
		*output_buffer = codepoint;
	} else if (codepoint < 0x800) {
		output_buffer = malloc(2 * sizeof(char));
		if (output_buffer == NULL) {
			goto MallocFailed;
		}
		b = output_buffer;
		*b++ = 192 + codepoint / 64;
		*b   = 128 + codepoint % 64;
	} else if (codepoint - 0xd800u < 0x800) {
		goto InvalidUnicode;
	} else if (codepoint < 0x10000) {
		output_buffer = malloc(3 * sizeof(char));
		if (output_buffer == NULL) {
			goto MallocFailed;
		}
		b = output_buffer;
		*b++ = 224 + codepoint / 4096;
		*b++ = 128 + codepoint / 64 % 64;
		*b   = 128 + codepoint % 64;
	} else if (codepoint < 0x110000) {
		output_buffer = malloc(4 * sizeof(char));
		if (output_buffer == NULL) {
			goto MallocFailed;
		}
		b = output_buffer;
		*b++ = 240 + codepoint / 262144;
		*b++ = 128 + codepoint / 4096 % 64;
		*b++ = 128 + codepoint / 64 % 64;
		*b   = 128 + codepoint % 64;
	} else {
		goto InvalidUnicode;
	}

	printf("%s", output_buffer);
	free(output_buffer);
	return 0;

	InvalidUnicode:
		fprintf(stderr, "Invalid Unicode code point: 0x%lx\n", codepoint);
		return RET_INVALID;

	MallocFailed:
		fprintf(stderr, "Failed to allocate memory!!\n");
		return RET_MALLOC;
}

int main(int argc, char *argv[]) {
	long codepoint;
	char *arg;
	char *strtol_err;
	size_t argi;
	int retval = 0;

	if (argc < 2) {
		return 0;
	}

	for (argi = 1 ; argi < argc ; argi++) {
		arg = argv[argi];
		if (*arg == '\0') {
			fprintf(stderr, "Empty argument.");
			return RET_USAGE;
		}

		// Set the locale to "C" to prohibit excessively fancy number representations.
		setlocale(LC_ALL, "C");
		// Explicitly set errno = 0
		errno = 0;
		codepoint = strtol(arg, &strtol_err, 16);
		if (*strtol_err != '\0' || errno != 0) {
			printf("%s", arg);
			//fprintf(stderr, "Invalid hexadecimal integer: %s\n", arg);
			//return RET_USAGE;
		} else if (codepoint < 0) {
			printf("%s", arg);
			//fprintf(stderr, "Not a positive integer: %s\n", arg);
			//return RET_INVALID;
		} else {
			retval = print_unicode((unsigned long)codepoint);
		}

		if (retval != 0) {
			return retval;
		}
	}
	printf("\n");
}
