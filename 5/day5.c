#include "..\aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint seeds[32]      = {0};
	uint seed_count     = 0;
	uint maps[7][40][3] = {0};
	uint map_sizes[7]   = {0};

	uint map_stack[128][4] = {0};
	uint map_stack_size    = 0;

	uint i = 0;

	i += sizeof("seeds:")-1;

	for (;;)
	{
		while (input.data[i] == ' ') ++i;
		if (input.data[i] == '\r') break;

		uint num = 0;
		for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);
		seeds[seed_count++] = num;
	}

	for (uint j = 0; j < 7; ++j)
	{
		while (input.data[i] < 0x20) ++i;
		while (input.data[i] != '\r') ++i;
		i += 2;

		for (;;)
		{
			for (uint k = 0; k < 3; ++k)
			{
				uint num = 0;
				for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);
				maps[j][map_sizes[j]][k] = num;
				i += (k < 2);
			}

			++map_sizes[j];

			i += 2;
			if (i >= input.size || input.data[i] == '\r') break;
			else                                          continue;
		}
	}

	uint part1_result = UINT_MAX;

	for (uint i = 0; i < seed_count; ++i)
	{
		uint val = seeds[i];
		for (uint j = 0; j < 7; ++j)
		{
			for (uint k = 0; k < map_sizes[j]; ++k)
			{
				if (val >= maps[j][k][1] && val < maps[j][k][1] + maps[j][k][2])
				{
					val = (val - maps[j][k][1]) + maps[j][k][0];
					break;
				}
			}
		}

		if (val < part1_result) part1_result = val;
	}

	printf("Part 1: %llu\n", part1_result);

	return 0;
}
