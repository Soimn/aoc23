#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint part1_result = 0;
	uint part2_result = 0;
	uint available_cubes[3] = { 12, 13, 14 };

	for (umm i = 0; i < input.size;)
	{
		i += sizeof("Game ")-1;

		uint id = 0;
		for (; (u8)(input.data[i] - 0x30) < (u8)10; ++i) id = id*10 + (input.data[i]&0xF);

		i += sizeof(": ")-1;

		uint max_amounts[3] = {0};

		bool is_possible = true;
		for (;;)
		{
			uint amounts[3] = {0};

			for (;;)
			{
				uint amount = 0;
				for (; (u8)(input.data[i] - 0x30) < (u8)10; ++i) amount = amount*10 + (input.data[i]&0xF);

				if (input.data[i + 1] == 'r')
				{
					amounts[0] = amount;
					i += sizeof(" red")-1;
				}
				else if (input.data[i + 1] == 'g')
				{
					amounts[1] = amount;
					i += sizeof(" green")-1;
				}
				else
				{
					amounts[2] = amount;
					i += sizeof(" blue")-1;
				}

				if (i < input.size && input.data[i] == ',')
				{
					i += sizeof(", ")-1;
					continue;
				}
				else break;
			}

			is_possible = (is_possible && (amounts[0] <= available_cubes[0] && amounts[1] <= available_cubes[1] && amounts[2] <= available_cubes[2]));
			for (uint j = 0; j < 3; ++j) if (amounts[j] > max_amounts[j]) max_amounts[j] = amounts[j];

			if (i < input.size && input.data[i] == ';')
			{
				i += sizeof("; ")-1;
				continue;
			}
			else break;
		}
		
		part1_result += (is_possible ? id : 0);
		part2_result += max_amounts[0]*max_amounts[1]*max_amounts[2];

		while (i < input.size && input.data[i] < 0x20) ++i;
	}

	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
