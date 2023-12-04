#include "..\aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint part1_result = 0;
	uint part2_result = 0;

	uint width = 0;
	while (input.data[width] != '\r') ++width;
	uint stride = width + 2;
	uint height = input.size/stride;

	sint deltas[8][2] = {
		{-1, -1},
		{ 0, -1},
		{ 1, -1},
		{-1,  0},
		{ 1,  0},
		{-1,  1},
		{ 0,  1},
		{ 1,  1},
	};

	for (uint y = 0; y*stride < input.size; ++y)
	{
		for (uint x = 0;;)
		{
			while ((u8)(input.data[y*stride + x]-0x30) >= (u8)10) ++x;
			if (x >= width) break;

			bool is_part  = false;
			uint part_num = 0;
			for (; (u8)(input.data[y*stride + x]-0x30) < (u8)10; ++x)
			{
				part_num = part_num*10 + (input.data[y*stride + x]&0xF);

				for (uint j = 0; j < sizeof(deltas)/sizeof(deltas[0]); ++j)
				{
					uint sx = (uint)((sint)x + deltas[j][0]);
					uint sy = (uint)((sint)y + deltas[j][1]);
					if (sx < width && sy < height)
					{
						is_part |= (input.data[sy*stride + sx] != '.' && (u8)(input.data[sy*stride + sx]-0x30) >= (u8)10);
					}
				}
			}

			part1_result += (is_part ? part_num : 0);
		}
	}

	for (uint i = 0;; ++i)
	{
		while (i < input.size && input.data[i] != '*') ++i;
		if (i >= input.size) break;

		uint part_acc    = 1;
		uint part_count  = 0;
		u8 delta_mask[9] = {0};
		for (uint j = 0; j < sizeof(deltas)/sizeof(deltas[0]); ++j)
		{
			uint sx = (uint)((sint)(i%stride) + deltas[j][0]);
			uint sy = (uint)((sint)(i/stride) + deltas[j][1]);
			if (delta_mask[j+j/5] == 0 && sx < width && sy < height && (u8)(input.data[sy*stride + sx]-0x30) < (u8)10)
			{
				uint px = sx;
				while (px > 0 && (u8)(input.data[sy*stride + px-1]-0x30) < (u8)10) --px;

				uint part_num = 0;
				for (; (u8)(input.data[sy*stride + px]-0x30) < (u8)10; ++px) part_num = part_num*10 + (input.data[sy*stride + px]&0xF);

				part_acc   *= part_num;
				part_count += 1;

				uint jj = j+j/5;
				uint kk = jj + (px-sx);
				if (kk > jj-jj%3 + 3) kk = jj-jj%3 + 3;
				for (uint k = jj; k < kk; ++k) delta_mask[k] = 1;
			}
		}

		if (part_count == 2) part2_result += part_acc;
	}

	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
