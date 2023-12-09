#include "../aoc.h"

typedef u32 uint32_t;
// https://burtleburtle.net/bob/hash/integer.html
uint32_t hash( uint32_t a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

// from Knuth Seminumerical Algorithms
uint
gcd(uint u, uint v)
{
	while (v != 0)
	{
		uint r = u % v;
		u = v;
		v = r;
	}

	return u;
}

uint
gcdn(uint* us, uint size)
{
	uint d = us[size-1];
	uint k = size-2;

	while (d != 1 && k > 0)
	{
		d = gcd(us[k], d);
		--k;
	}

	return d;
}

uint
lcm(uint u, uint v)
{
	return (v*u)/gcd(u, v);
}

uint
lcmn(uint* us, uint size)
{
	uint result = us[0];
	for (uint i = 1; i < size; ++i) result = lcm(result, us[i]);
	return result;
}

int
main(int argc, char** argv)
{
	String input = {0};
	if (!ReadInput(argc, argv, &input)) return 0;

	u64 directions[5]    = {0};
	uint direction_count = 0;

	u32 (*nodes)[2] = malloc(sizeof(uint[2])*1000);
	uint node_count = 1; // NOTE: 0 is used as null

	u64 node_lut[1024][4] = {0};

	sint start_node_idx = -1;
	sint end_node_idx   = -1;

	uint start_node_idxs[6];
	uint start_node_idx_count = 0;
	uint end_node_idxs[6];
	uint end_node_idx_count = 0;

	uint i = 0;

	for (; input.data[i] != '\r'; ++i, ++direction_count) directions[direction_count >> 6] |= (uint)(input.data[i] == 'R') << (direction_count & 63);

	while (input.data[i] <= 0x20) ++i;

	while (i < input.size)
	{
		u32 node = input.data[i] | ((u32)input.data[i+1] << 8) | ((u32)input.data[i+2] << 16);
		i += sizeof("XXX = (")-1;
		u32 left = input.data[i] | ((u32)input.data[i+1] << 8) | ((u32)input.data[i+2] << 16);
		i += sizeof("XXX, ")-1;
		u32 right = input.data[i] | ((u32)input.data[i+1] << 8) | ((u32)input.data[i+2] << 16);
		i += sizeof("XXX)\r\n")-1;

		if      (node == 0x414141) start_node_idx = node_count;
		else if (node == 0x5A5A5A) end_node_idx   = node_count;

		if      (node >> 16 == 'A') start_node_idxs[start_node_idx_count++] = node_count;
		else if (node >> 16 == 'Z') end_node_idxs[end_node_idx_count++]     = node_count;

		uint node_lut_idx = hash(node) % ARRAY_SIZE(node_lut);
		uint j = 0;
		for (; j < ARRAY_SIZE(node_lut[node_lut_idx]) && node_lut[node_lut_idx][j] != 0; ++j);

		ASSERT(j < ARRAY_SIZE(node_lut[node_lut_idx]));
		node_lut[node_lut_idx][j] = node | ((u64)node_count << 32);

		nodes[node_count][0] = left;
		nodes[node_count][1] = right;

		++node_count;
	}

	for (uint j = 0; j < node_count; ++j)
	{
		u32 left  = nodes[j][0];
		u32 right = nodes[j][1];

		u32 left_lut_idx  = hash(left)  % ARRAY_SIZE(node_lut);
		u32 right_lut_idx = hash(right) % ARRAY_SIZE(node_lut);

		u32 left_idx;
		for (uint k = 0; k < ARRAY_SIZE(node_lut[left_lut_idx]); ++k)
		{
			if ((u32)node_lut[left_lut_idx][k] == left)
			{
				left_idx = (u32)(node_lut[left_lut_idx][k] >> 32);
			}
		}

		u32 right_idx;
		for (uint k = 0; k < ARRAY_SIZE(node_lut[right_lut_idx]); ++k)
		{
			if ((u32)node_lut[right_lut_idx][k] == right)
			{
				right_idx = (u32)(node_lut[right_lut_idx][k] >> 32);
			}
		}

		nodes[j][0] = left_idx;
		nodes[j][1] = right_idx;
	}

	uint part1_result = 0;
	if (start_node_idx != -1 && end_node_idx != -1)
	{
		for (u32 node_idx = (u32)start_node_idx; node_idx != (u32)end_node_idx; ++part1_result)
		{
			uint step_idx = part1_result % direction_count;
			uint step     = !!(directions[step_idx >> 6] & (1ULL << (step_idx & 63)));
			node_idx = nodes[node_idx][step];
		}
	}

	uint cycle_scan[6][10] = {0};
	uint cycle_scan_size[6] = {0};
	uint cycle_scan_count = 0;

	uint scan_nodes[6];
	uint scan_node_count = start_node_idx_count;
	for (uint j = 0; j < scan_node_count; ++j) scan_nodes[j] = start_node_idxs[j];

	uint part2_result = 0;
	uint ends = 0;
	for (; ends < end_node_idx_count && cycle_scan_count != 6; ++part2_result)
	{
		ends = 0;

		uint step_idx = part2_result % direction_count;
		uint step     = !!(directions[step_idx >> 6] & (1ULL << (step_idx & 63)));

		for (uint j = 0; j < scan_node_count; ++j)
		{
			scan_nodes[j] = nodes[scan_nodes[j]][step];

			for (uint k = 0; k < end_node_idx_count; ++k)
			{
				if (scan_nodes[j] == end_node_idxs[k])
				{
					if (cycle_scan_size[j] < ARRAY_SIZE(cycle_scan[j]))
					{
						cycle_scan[j][cycle_scan_size[j]++] = part2_result;
						cycle_scan_count += (cycle_scan_size[j] == ARRAY_SIZE(cycle_scan[0]));
					}
					++ends;
					break;
				}
			}
		}
	}

	if (ends != scan_node_count)
	{
		uint cycle_lens[ARRAY_SIZE(cycle_scan)];
		for (uint k = 0; k < ARRAY_SIZE(cycle_scan); ++k)
		{
			for (uint j = ARRAY_SIZE(cycle_scan[0])-1; j > 0; --j)
			{
				cycle_scan[k][j] -= cycle_scan[k][j-1];
			}

			cycle_lens[k] = cycle_scan[k][ARRAY_SIZE(cycle_scan[k])-1];
		}

		part2_result = lcmn(cycle_lens, ARRAY_SIZE(cycle_lens));
	}

	if (part1_result == 0) printf("Part 1: ---\n");
	else                   printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
