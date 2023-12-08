#include "../aoc.h"

#include <math.h>

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	uint times[4];
	uint distances[4];
	uint race_count = 0;

	{
		uint i = 0;

		i += sizeof("Time:")-1;

		for (;;)
		{
			while (input.data[i] == ' ') ++i;
			if (input.data[i] == '\r') break;

			uint num = 0;
			for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);

			times[race_count++] = num;
		}

		i += sizeof("\r\nDistance:")-1;

		for (uint j = 0; j < race_count; ++j)
		{
			while (input.data[i] == ' ') ++i;

			uint num = 0;
			for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);

			distances[j] = num;
		}
	}

	uint part1_result = 1;
	for (uint i = 0; i < race_count; ++i)
	{
		float td2 = times[i]/2.0f;
		float dev = sqrtf(td2*td2 - distances[i] - 0.1f);
		
		uint max = (uint)(td2+dev);
		uint min = (uint)(ceilf(td2-dev));

		while (max*(times[i] - max) <= distances[i]) --max;
		while (min*(times[i] - min) <= distances[i]) ++min;

		part1_result *= max - min + 1;
	}

	uint time     = 0;
	uint distance = 0;
	{
		uint i = 0;

		i += sizeof("Time:")-1;

		for (;;)
		{
			while (input.data[i] == ' ') ++i;
			if (input.data[i] == '\r') break;

			for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) time = time*10 + (input.data[i]&0xF);
		}

		i += sizeof("\r\nDistance:")-1;

		for (uint j = 0; j < race_count; ++j)
		{
			while (input.data[i] == ' ') ++i;

			for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) distance = distance*10 + (input.data[i]&0xF);
		}
	}

	float td2 = time/2.0f;
	float dev = sqrtf(td2*td2 - distance - 0.1f);
	
		uint max = (uint)(td2+dev);
		uint min = (uint)(ceilf(td2-dev));

		while (max*(time - max) <= distance) --max;
		while (min*(time - min) <= distance) ++min;

		uint part2_result = max - min + 1;

	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
