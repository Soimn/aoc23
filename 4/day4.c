#include "..\aoc.h"

void
Quicksort(u8* arr, uint size)
{
	if (size <= 1) return;

	u8 pivot = arr[size-1];

	uint swap_cursor = 0;
	for (uint scan = 0; scan < size; ++scan)
	{
		if (arr[scan] <= pivot)
		{
			u8 tmp = arr[swap_cursor];
			arr[swap_cursor] = arr[scan];
			arr[scan]        = tmp;

			swap_cursor += 1;
		}
	}

	Quicksort(arr, swap_cursor-1);
	Quicksort(arr + swap_cursor, size - swap_cursor);
}

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint part1_result = 0;
	uint part2_result = 0;

	uint copy_count[201] = {0};
	for (uint i = 0; i < input.size;)
	{
		u8 winning_numbers[10] = {0};
		u8 card_numbers[25]    = {0};
		uint winning_num_count = 0;
		uint card_num_count    = 0;

		i += sizeof("Card")-1;
		while (input.data[i] == ' ') ++i;

		uint idx = 0;
		for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) idx = idx*10 + (input.data[i]&0xF);
		idx -= 1;

		i += sizeof(":")-1;

		u8* numbers = winning_numbers;
		uint count  = 0;
		for (;;)
		{
			while (input.data[i] == ' ') ++i;

			if (input.data[i] == '\r')
			{
				while (input.data[i] <= 0x20) ++i;
				break;
			}
			else if (input.data[i] == '|')
			{
				winning_num_count = count;

				numbers = card_numbers;
				count   = 0;

				++i;
				continue;
			}
			else
			{
				u8 num = 0;
				for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);

				numbers[count++] = num;
			}
		}

		card_num_count = count;

		Quicksort(winning_numbers, winning_num_count);
		Quicksort(card_numbers, card_num_count);

		uint wins = 0;
		for (uint j = 0; j < card_num_count; ++j)
		{
			uint card_num = card_numbers[j];

			uint a = 0;
			uint b = winning_num_count-1;

			// TODO: Branchless
			while (a < b)
			{
				uint c = (a+b)/2;

				if (winning_numbers[c] >= card_num)
				{
					b = c;
				}
				else
				{
					a = c + 1;
				}
			}

			ASSERT(a == b);
			if (winning_numbers[a] == card_num) ++wins;
		}

		if (wins != 0) part1_result += (1ULL << (wins-1));

		copy_count[idx] += 1;

		uint max_k = idx+1+wins;
		if (max_k > ARRAY_SIZE(copy_count)) max_k = ARRAY_SIZE(copy_count);
		for (uint k = idx+1; k < max_k; ++k)
		{
			copy_count[k] += copy_count[idx];
		}
	}

	for (uint i = 0; i < ARRAY_SIZE(copy_count); ++i) part2_result += copy_count[i];

	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
