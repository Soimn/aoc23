#include "..\aoc.h"

void
AddRange(uint range_stack[128][2], uint* range_stack_size, uint mapped_range_start, uint mapped_range_end)
{
	range_stack[*range_stack_size][0] = mapped_range_start;
	range_stack[*range_stack_size][1] = mapped_range_end;
	*range_stack_size += 1;

	for (sint k = 0; k < (sint)*range_stack_size-1; ++k)
	{
		uint check_range_start = range_stack[k][0];
		uint check_range_end   = range_stack[k][0];
		uint main_range_start  = range_stack[*range_stack_size-1][0];
		uint main_range_end    = range_stack[*range_stack_size-1][1];

		if (check_range_start <= main_range_end && check_range_end >= main_range_start)
		{
			uint new_range_start = (check_range_start < main_range_start ? check_range_start : main_range_start);
			uint new_range_end   = (check_range_end   > main_range_end   ? check_range_end   : main_range_end);

			range_stack[k][0] = range_stack[*range_stack_size-2][0];
			range_stack[k][1] = range_stack[*range_stack_size-2][1];
			*range_stack_size -= 1;

			range_stack[*range_stack_size-1][0] = new_range_start;
			range_stack[*range_stack_size-1][1] = new_range_end;
		
			--k;
		}
	}
}

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint seeds[32]      = {0};
	uint seed_count     = 0;
	uint maps[7][40][3] = {0};
	uint map_sizes[7]   = {0};

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

	uint range_stack[2][128][2];
	uint range_stack_sizes[2] = {0};

	for (uint i = 0; i < seed_count; i += 2) AddRange(range_stack[0], &range_stack_sizes[0], seeds[i], seeds[i] + seeds[i+1]-1);

	uint old_idx = 0;
	for (uint i = 0; i < 7; ++i, old_idx = !old_idx)
	{
		uint new_idx = !old_idx;

		for (uint h = 0; h < range_stack_sizes[old_idx]; ++h)
		{
			uint range_start = range_stack[old_idx][h][0];
			uint range_end   = range_stack[old_idx][h][1];
			for (uint j = 0; j < map_sizes[i]; ++j)
			{
				uint map_start = maps[i][j][1];
				uint map_end   = map_start + maps[i][j][2]-1;
				if (range_start <= map_end && range_end >= map_start)
				{
					if (range_end > map_end)
					{
						if (range_start < map_start) // range contains map
						{
							range_stack[old_idx][h][0] = range_start;
							range_stack[old_idx][h][1] = map_start-1;
							range_stack[old_idx][range_stack_sizes[old_idx]][0] = map_end+1;
							range_stack[old_idx][range_stack_sizes[old_idx]][1] = range_end;
							++range_stack_sizes[old_idx];

							range_start = map_start;
							range_end   = map_end;
						}
						else                         // range overlaps end of map
						{
							range_stack[old_idx][h][0] = map_end+1;
							range_stack[old_idx][h][1] = range_end;

							range_end = map_end;
						}
					}
					else
					{
						if (range_start < map_start) // range overlaps start of map
						{
							range_stack[old_idx][h][0] = range_start;
							range_stack[old_idx][h][1] = map_start-1;

							range_start = map_start;
						}
						else                         // map contains range
						{
							range_stack[old_idx][h][0] = range_stack[old_idx][range_stack_sizes[old_idx]-1][0];
							range_stack[old_idx][h][1] = range_stack[old_idx][range_stack_sizes[old_idx]-1][1];
							--range_stack_sizes[old_idx];
						}
					}

					uint mapped_range_start = (range_start - map_start) + maps[i][j][0];
					uint mapped_range_end   = (range_end   - map_start) + maps[i][j][0];

					AddRange(range_stack[new_idx], &range_stack_sizes[new_idx], mapped_range_start, mapped_range_end);
					--h;
				}
			}
		}

		for (uint k = 0; k < range_stack_sizes[old_idx]; ++k)
		{
			AddRange(range_stack[new_idx], &range_stack_sizes[new_idx], range_stack[old_idx][k][0], range_stack[old_idx][k][1]);
		}
	}

	uint part2_result = UINT_MAX;
	for (uint i = 0; i < range_stack_sizes[old_idx]; ++i)
	{
		if (range_stack[old_idx][i][0] < part2_result) part2_result = range_stack[old_idx][i][0];
	}

	printf("Part 2: %llu\n", part2_result);

	return 0;
}
