#include "../aoc.h"

#include <windows.h>

int
main(int argc, char** argv)
{
	String original_input = {0};
	if (!ReadInputAligned(argc, argv, &original_input, 32)) return 0;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start_time;
	QueryPerformanceCounter(&start_time);

	String input = original_input;
#if 1
	uint result = 0;
	while (input.size != 0)
	{
		__m256i ic0 = _mm256_loadu_si256((__m256i*)input.data);
		__m256i ic1 = _mm256_loadu_si256((__m256i*)(input.data + 32));

		__m256i whitespace_thresh = _mm256_set1_epi8(0x20);
		s32 ic0_ws_mm = ~_mm256_movemask_epi8(_mm256_cmpgt_epi8(ic0, whitespace_thresh));
		s32 ic1_ws_mm = ~_mm256_movemask_epi8(_mm256_cmpgt_epi8(ic1, whitespace_thresh));

		__m256i digit_push   = _mm256_set1_epi8(0x7F - 0x39);
		__m256i digit_thresh = _mm256_set1_epi8(127 - 10);
		ic0 = _mm256_add_epi8(ic0, digit_push);
		ic1 = _mm256_add_epi8(ic1, digit_push);
		s32 ic0_digit_mm = _mm256_movemask_epi8(_mm256_cmpgt_epi8(ic0, digit_thresh));
		s32 ic1_digit_mm = _mm256_movemask_epi8(_mm256_cmpgt_epi8(ic1, digit_thresh));

		u32 ic0_ws_idx = 0;
		u32 ic1_ws_idx = 0;
		bool ic0_has_ws = _BitScanForward(&ic0_ws_idx, ic0_ws_mm);
		bool ic1_has_ws = _BitScanForward(&ic1_ws_idx, ic1_ws_mm);

		uint line_length;
		u64 digit_mask;
		if (ic0_has_ws)
		{
			line_length = ic0_ws_idx;

			digit_mask = (u64)(u32)((ic0_ws_mm - 1) & ic0_digit_mm);
		}
		else
		{
			line_length = 32 + ic1_ws_idx;
			
			digit_mask = (u64)(u32)ic0_digit_mm | ((u64)((ic1_ws_mm - 1) & ic1_digit_mm) << 32);
		}

		u32 digit0_idx;
		u32 digit1_idx;
		_BitScanForward64(&digit0_idx, digit_mask);
		_BitScanReverse64(&digit1_idx, digit_mask);

		result += (input.data[digit0_idx]&0xF)*10 + (input.data[digit1_idx]&0xF);

		input.data += line_length + 2;
		input.size -= line_length + 2; // NOTE: This is kind of sketchy, but works with the input format
	}
#else
	umm result = 0;
	while (input.size != 0)
	{
		smm first_digit_idx = -1;
		smm last_digit_idx  = -1;
		umm i = 0;
		for (; input.data[i] != '\r'; ++i)
		{
			if ((u8)(input.data[i]-0x30) < (u8)10)
			{
				if (first_digit_idx == -1) first_digit_idx = i;
				last_digit_idx = i;
			}
		}

		result += (input.data[first_digit_idx]&0xF)*10 + (input.data[last_digit_idx]&0xF);
		input.data += i + 2;
		input.size -= i + 2;
	}
#endif

	LARGE_INTEGER end_time;
	QueryPerformanceCounter(&end_time);

	f32 us = (f32)((end_time.QuadPart - start_time.QuadPart)*1000000)/freq.QuadPart;

	printf("Part 1: %llu in %f us\n", result, us);

#if 0
	input  = original_input;
	result = 0;
	while (input.size != 0)
	{
		smm first_digit = -1;
		smm last_digit  = -1;
		umm i = 0;
		for (; input.data[i] != '\r'; ++i)
		{
			smm digit = -1;

			if ((u8)(input.data[i]-0x30) < (u8)10) digit = input.data[i]&0xF;
			else
			{
				String digits[9] = {
					STRING("one"), STRING("two"), STRING("three"),
					STRING("four"), STRING("five"), STRING("six"),
					STRING("seven"), STRING("eight"), STRING("nine")
				};

				for (umm j = 0; j < 9; ++j)
				{
					String s = digits[j];
					umm k = 0;
					for (; k < s.size && input.data[i + k] == s.data[k]; ++k);

					if (k == s.size)
					{
						digit = j+1;
						break;
					}
				}
			}

			if (digit != -1)
			{
				if (first_digit == -1) first_digit = digit;
				last_digit = digit;
			}
		}

		result += first_digit*10 + last_digit;

		input.data += i + 2;
		input.size -= i + 2;
	}

	printf("Part 2: %llu\n", result);
#endif
}
