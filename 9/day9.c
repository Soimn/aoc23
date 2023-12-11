#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input = {0};
	if (!ReadInput(argc, argv, &input)) return 0;

	uint sequences[200][22];
	uint seq_count = 0;

	uint seq_len = 1;
	for (uint i = 0; input.data[i] != '\r'; ++i) seq_len += (input.data[i] == ' ');

	for (uint i = 0; i < input.size; ++seq_count)
	{
		for (uint j = 0; j < seq_len; ++j)
		{
			sint sign = 1;
			if (input.data[i] == '-')
			{
				sign = -1;
				++i;
			}

			sint num  = 0;
			for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);

			sequences[seq_count][j] = num*sign;

			i += sizeof(" ")-1;
		}

		i += sizeof("\n")-1; // NOTE: \r is dealt with by the loop
	}

	sint part1_result = 0;
	sint part2_result = 0;

	for (uint i = 0; i < seq_count; ++i)
	{
		uint start_diffs[21];
		for (uint j = seq_len-1; j > 0; --j)
		{
			start_diffs[(seq_len-1)-j] = sequences[i][0];

			for (uint k = 0; k < j; ++k)
			{
				sequences[i][k] = sequences[i][k+1] - sequences[i][k];
			}
		}

		start_diffs[seq_len-1] = sequences[i][0];

		sint sum = 0;
		for (uint j = 0; j < seq_len; ++j) sum += sequences[i][j];

		part1_result += sum;

		sint x = 0;
		for (uint j = seq_len-2; j < seq_len; --j)
		{
			x = start_diffs[j] - x;
		}

		part2_result += x;
	}

	printf("Part 1: %lld\n", part1_result);
	printf("Part 2: %lld\n", part2_result);

	return 0;
}
