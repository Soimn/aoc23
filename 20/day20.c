#include "../aoc.h"

#include <string.h>

enum
{
	NodeKind_None = 0,
	NodeKind_FlipFlop,
	NodeKind_Conjunction,
};

typedef struct Node
{
	u16 connections[8];
	u8 connections_len;
	u8 kind;
	union
	{
		u8 state[8];
		u64 state_u64;
	};
} Node;

typedef struct Node_Mapping
{
	u16 id;
	u16 idx;
} Node_Mapping;

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return -1;

	Node nodes[256] = {0};
	uint nodes_len  = 0;

	Node_Mapping mappings[256] = {0};
	uint mappings_len          = 0;

	for (umm i = 0; i < input.size;)
	{
		ASSERT(nodes_len < ARRAY_SIZE(nodes));
		ASSERT(mappings_len < ARRAY_SIZE(mappings));
		Node* node            = &nodes[nodes_len++];
		Node_Mapping* mapping = &mappings[mappings_len++];

		*node = (Node){0};

		mapping->idx = nodes_len-1;

		if      (input.data[i] == '%') node->kind = NodeKind_FlipFlop;
		else if (input.data[i] == '&') node->kind = NodeKind_Conjunction;

		if (node->kind != NodeKind_None)
		{
			i += 1; // NOTE: skip kind prefix
		
			ASSERT(i < input.size);
			while (i < input.size && (u8)(input.data[i]-'a') < (u8)26)
			{
				mapping->id *= 26;
				mapping->id += input.data[i]-'a';
				i += 1;
			}
		}
		else
		{
			ASSERT(i + sizeof("broadcaster")-1 < input.size && input.data[i + sizeof("broadcaster")-1] == ' ');
			ASSERT(strncmp((char*)(input.data + i), "broadcaster", sizeof("broadcaster")-1) == 0);

			mapping->id = 0;

			i += sizeof("broadcaster")-1;
		}

		ASSERT(i + sizeof(" -> ")-1 < input.size && strncmp((char*)(input.data + i), " -> ", sizeof(" -> ")-1) == 0);
		i += sizeof(" -> ")-1;

		for (;;)
		{
			ASSERT(i < input.size);

			u16 id = 0;
			while (i < input.size && (u8)(input.data[i]-'a') < (u8)26)
			{
				id *= 26;
				id += input.data[i]-'a';
				i += 1;
			}

			node->connections[node->connections_len++] = id;

			ASSERT(i+1 < input.size);
			if (input.data[i] == ',')
			{
				ASSERT(input.data[i+1] == ' ');
				i += sizeof(", ")-1;
				continue;
			}
			else
			{
				ASSERT(input.data[i] == '\r' && input.data[i+1] == '\n');
				i += sizeof("\r\n")-1;
				break;
			}
		}
	}

	for (uint i = 0; i < nodes_len; ++i)
	{
		Node* node = &nodes[i];
		for (uint j = 0; j < node->connections_len; ++j)
		{
			u16 id = node->connections[j];

			sint idx = -1;
			for (uint k = 0; k < mappings_len; ++k)
			{
				if (mappings[k].id == id)
				{
					idx = (sint)mappings[k].idx;
					break;
				}
			}

			ASSERT(idx != -1 && idx < U16_MAX);

			node->connections[j] = (u16)idx;
		}
	}

	typedef struct Signal
	{
		u16 src;
		u16 dst;
		u8 val;
	} Signal;

	Signal signals[ARRAY_SIZE(nodes)*ARRAY_SIZE(nodes[0].connections)];
	uint signals_head = 0;
	uint signals_tail = 0;

	uint lo_pulse_count = 0;
	uint hi_pulse_count = 0;
	for (;;)
	{
		for (uint i = 0; i < nodes[0].connections_len; ++i)
		{
			signals[signals_head] = (Signal){ .src = 0, .dst = nodes[0].connections[i], .val = 0 };
			signals_head = (signals_head+1) % ARRAY_SIZE(signals);
		}

		lo_pulse_count += nodes[0].connections_len;

		while (signals_tail != signals_head)
		{
			Signal signal = signals[signals_tail];
			signals_tail = (signals_tail+1) % ARRAY_SIZE(signals);

			Node* dst_node = &nodes[signal.dst];

			bool should_send_signal = false;
			u16 signal_val          = 0;

			if (dst_node->kind == NodeKind_FlipFlop)
			{
				if (signal.val == 0)
				{
					dst_node->state[0] = !dst_node->state[0];

					should_send_signal = true;
					signal_val         = dst_node->state[0];
				}
			}
			else if (dst_node->kind == NodeKind_Conjunction)
			{
				u16 val = 1;
				for (uint j = 0; j < dst_node->connections_len; ++j)
				{
					if (dst_node->connections[j] == signal.src)
					{
						dst_node->state[j] = signal.val;
					}

					val &= dst_node->state[j];
				}

				should_send_signal = true;
				signal_val         = val;
			}

			if (should_send_signal)
			{
				for (uint j = 0; j < dst_node->connections_len; ++j)
				{
					signals[signals_head] = (Signal){ .src = signal.dst, .dst = dst_node->connections[j], .val = signal_val };
					signals_head = (signals_head+1) % ARRAY_SIZE(signals);
					ASSERT(signals_head != signals_tail);
				}

				if (signal_val == 0) lo_pulse_count += dst_node->connections_len;
				else                 hi_pulse_count += dst_node->connections_len;
			}
		}

		bool is_in_init_state = true;
		for (uint i = 0; i < nodes_len && is_in_init_state; ++i)
		{
			is_in_init_state = (nodes[i].state_u64 == 0);
		}

		if (is_in_init_state) break;
		else                  continue;
	}

	printf("%llu, %llu\n", lo_pulse_count, hi_pulse_count);

	uint part1_result = lo_pulse_count*hi_pulse_count*1000;
	printf("Part 1: %llu\n", part1_result);

	return 0;
}
