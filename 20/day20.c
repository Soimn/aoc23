#include "../aoc.h"

enum
{
	NodeKind_None = 0,
	NodeKind_FlipFlop,
	NodeKind_Conjunction,
};

typedef struct Node
{
	String name;
	u8 kind;
	u8 connections_len;
	u8 connections[8];
	u8 sources_len;
	u8 sources[16];
	u8 state[16];
} Node;

typedef struct Signal
{
	u8 src;
	u8 dst;
	u8 val;
} Signal;

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return -1;

	Node nodes[256] = {0};
	uint nodes_len = 0;

	{
		umm i = 0;
		while (i < input.size)
		{
			ASSERT(nodes_len < ARRAY_SIZE(nodes));
			Node* node = &nodes[nodes_len++];

			node->kind = NodeKind_None;
			if      (input.data[i] == '%') node->kind = NodeKind_FlipFlop;
			else if (input.data[i] == '&') node->kind = NodeKind_Conjunction;

			if (node->kind != NodeKind_None) i += 1;

			node->name.data = &input.data[i];
			while (i < input.size && input.data[i] != ' ') i += 1;
			node->name.size = &input.data[i] - node->name.data;

			while (i < input.size && input.data[i] != '\n') i += 1;
			i += 1;
		}
	}

	{
		umm i = 0;
		umm j = 0;
		while (i < input.size)
		{
			Node* node = &nodes[j++];

			if (node->kind != NodeKind_None) i += 1;
			i += node->name.size;

			ASSERT(i+3 < input.size && input.data[i] == ' ' && input.data[i+1] == '-' && input.data[i+2] == '>' && input.data[i+3] == ' ');
			i += 4;

			node->connections_len = 0;
			for (;;)
			{
				ASSERT(i < input.size);
				String name;
				name.data = &input.data[i];
				while (i < input.size && input.data[i] != ',' && input.data[i] != '\r') i += 1;
				name.size = &input.data[i] - name.data;

				sint idx = -1;

				if (String_Match(name, STRING("rx"))) idx = 0xFE;
				else
				{
					for (uint k = 0; k < nodes_len; ++k)
					{
						if (String_Match(nodes[k].name, name))
						{
							idx = (sint)k;
							break;
						}
					}
				}

				node->connections[node->connections_len++] = (u8)idx;

				ASSERT(nodes[idx].sources_len < ARRAY_SIZE(nodes[idx].sources));
				nodes[idx].sources[nodes[idx].sources_len++] = j-1;

				if (i < input.size && input.data[i] == ',')
				{
					i += sizeof(", ")-1;
					continue;
				}
				else
				{
					i += sizeof("\r\n")-1;
					break;
				}
			}
		}
	}

	uint lo_signals[1001] = {0};
	uint hi_signals[1001] = {0};

	uint broadcaster_node = 0;
	while (broadcaster_node < nodes_len && !String_Match(nodes[broadcaster_node].name, STRING("broadcaster"))) broadcaster_node += 1;
	ASSERT(broadcaster_node < nodes_len);

	uint k = 0;
	uint cycle_len = 0;
	sint part2_result = -1;
	while (k < 1000 || part2_result == -1)
	{
		k += 1;
		if (k % 10000000 == 0)
		{
			fprintf(stderr, "%llu\n", k);
		}

		if (k < 1000) cycle_len += 1;

		Signal signals[ARRAY_SIZE(nodes)*ARRAY_SIZE(nodes[0].connections)];
		uint signals_head = 0;
		uint signals_tail = 0;

		for (uint i = 0; i < nodes[broadcaster_node].connections_len; ++i)
		{
			if (nodes[broadcaster_node].connections[i] >= 0xFE) continue;
			signals[signals_head] = (Signal){ .src = broadcaster_node, .dst = nodes[broadcaster_node].connections[i], .val = 0 };
			signals_head = (signals_head+1) % ARRAY_SIZE(signals);
			ASSERT(signals_head != signals_tail);
		}

		if (cycle_len <= 1000)
		{
			lo_signals[cycle_len] = lo_signals[cycle_len-1] + nodes[broadcaster_node].connections_len + 1;
			hi_signals[cycle_len] = hi_signals[cycle_len-1];
		}

		while (signals_tail != signals_head)
		{
			Signal sig = signals[signals_tail];
			signals_tail = (signals_tail+1) % ARRAY_SIZE(signals);

			Node* dst_node = &nodes[sig.dst];

			bool should_send_signal_out = false;
			u8 out_val                  = 0;

			if (dst_node->kind == NodeKind_FlipFlop)
			{
				if (sig.val == 0)
				{
					dst_node->state[0] = !dst_node->state[0];

					should_send_signal_out = true;
					out_val = dst_node->state[0];
				}
			}
			else if (dst_node->kind == NodeKind_Conjunction)
			{
				u8 val = 1;
				for (uint i = 0; i < dst_node->sources_len; ++i)
				{
					if (dst_node->sources[i] == sig.src)
					{
						dst_node->state[i] = sig.val;
					}

					val &= dst_node->state[i];
				}

				should_send_signal_out = true;
				out_val                = !val;
			}

			if (should_send_signal_out)
			{
				for (uint i = 0; i < dst_node->connections_len; ++i)
				{
					if (out_val == 0 && dst_node->connections[i] == 0xFE && part2_result == -1) part2_result = (sint)k;

					if (dst_node->connections[i] >= 0xFE) continue;

					Signal out_signal = (Signal){ .src = sig.dst, .dst = dst_node->connections[i], .val = out_val };

					signals[signals_head] = out_signal;
					signals_head = (signals_head+1) % ARRAY_SIZE(signals);
					ASSERT(signals_head != signals_tail);
				}

				if (cycle_len < 1000)
				{
					if (out_val == 0) lo_signals[cycle_len] += dst_node->connections_len;
					else              hi_signals[cycle_len] += dst_node->connections_len;
				}
			}
		}

		bool is_in_init_state = true;
		for (uint i = 0; i < nodes_len && is_in_init_state; ++i)
		{
			for (uint j = 0; j < ARRAY_SIZE(nodes[i].state); ++j)
			{
				is_in_init_state = (is_in_init_state && nodes[i].state[j] == 0);
			}
		}

		if (is_in_init_state) break;
		else                  continue;
	}

	uint whole_cycles = 1000/cycle_len;
	uint remainder    = 1000%cycle_len;

	uint total_lo_signals = lo_signals[cycle_len]*whole_cycles + lo_signals[remainder];
	uint total_hi_signals = hi_signals[cycle_len]*whole_cycles + hi_signals[remainder];

	printf("Part 1: %llu\n", total_lo_signals*total_hi_signals);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
