#include "../aoc.h"

// bit layout 0000WESN
enum Pipe_Kind
{
	Pipe_None  = 0,
	Pipe_NS    = 0x3,
	Pipe_EW    = 0xC,
	Pipe_NW    = 0x9,
	Pipe_NE    = 0x5,
	Pipe_SW    = 0xA,
	Pipe_SE    = 0x6,
	Pipe_Start = 0x10,
};

int
main(int argc, char** argv)
{
	String input = {0};
	if (!ReadInput(argc, argv, &input)) return 0;

	u8 map[140*140] = {0};
	uint map_width  = 0;
	uint map_height = 0;
	uint sx = 0, sy = 0;

	for (uint i = 0; input.data[i] != '\r'; ++i) ++map_width;

	for (uint j = 0; j*(map_width+2) < input.size; ++j, ++map_height)
	{
		for (uint i = 0; i < map_width; ++i)
		{
			u8 pipe_kind;
			switch (input.data[j*(map_width + 2) + i])
			{
				case 'S': pipe_kind = Pipe_Start; break;
				case '|': pipe_kind = Pipe_NS;    break;
				case '-': pipe_kind = Pipe_EW;    break;
				case 'J': pipe_kind = Pipe_NW;    break;
				case 'L': pipe_kind = Pipe_NE;    break;
				case '7': pipe_kind = Pipe_SW;    break;
				case 'F': pipe_kind = Pipe_SE;    break;
				default:  pipe_kind = Pipe_None;  break;
			}

			map[j*map_width + i] = pipe_kind;
			
			if (pipe_kind == Pipe_Start)
			{
				sx = i;
				sy = j;
			}
		}
	}

	uint x, y, start_walk_dir;

	for (uint i = 0; i < 4; ++i)
	{
		uint deltas = 0xFF010000000001FF >> (i << 3);

		x        = sx + ((s8*)&deltas)[4];
		y        = sy + ((s8*)&deltas)[0];
		start_walk_dir = 1ULL << i;

		if (x < map_width && y < map_height && (map[y*map_width + x] & (0x88 >> (i+2))) != 0) break;
	}

	uint walk_dir = start_walk_dir;
	map[sy*map_width+sx] = Pipe_Start | (u8)walk_dir;

	uint loop_length = 1;
	for (; (map[y*map_width + x]&0xF0) != Pipe_Start; ++loop_length)
	{
		map[y*map_width + x] |= 0x80;

		walk_dir = (u8)(0x0400000008000102>>((walk_dir-1)<<3));
		walk_dir = (map[y*map_width + x] & ~walk_dir) & 0xF;

		x += (s8)(0xFF0100 >> ((walk_dir>>2)<<3));
		y += (s8)(0x01FF00 >> ((walk_dir&0x3)<<3));
	}

	walk_dir = (u8)(0x0400000008000102>>((walk_dir-1)<<3));
	map[sy*map_width+sx] = 0x80 | (u8)walk_dir | (u8)start_walk_dir;

	uint part2_result = 0;
	for (uint j = 0; j < map_height; ++j)
	{
		uint crossings  = 0;
		u8 last_crossed = 0;
		for (uint i = 0; i < map_width; ++i)
		{
			u8 cell = map[j*map_width + i];
			if ((cell&0x80) && (cell&0x3))
			{
				crossings += !((last_crossed&0xF) == Pipe_SE && (cell&0xF) == Pipe_NW || (last_crossed&0xF) == Pipe_NE && (cell&0xF) == Pipe_SW);
				last_crossed = cell;
			}

			if ((cell&0x80) == 0 && crossings%2 != 0)
			{
				part2_result += 1;
				map[j*map_width+i] |= 0x40;
			}
		}
	}

	uint part1_result = loop_length/2;
	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	for (uint j = 0; j < map_height; ++j)
	{
		for (uint i = 0; i < map_width; ++i)
		{
			printf((map[j*map_width+i]&0x40) ? "1" : "0");
		}
		printf("\n");
	}

	return 0;
}
