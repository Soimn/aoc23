#include "../aoc.h"

typedef struct Workflow
{
	u16 id;
	u16 first_rule;
} Workflow;

typedef struct Rule
{
	u8 var_idx;
	s8 cmp_mask;
	s16 val;
	s16 true_child;
	s16 false_child;
} Rule;

typedef union Part_Range
{
	struct
	{
		u16 a_min;
		u16 m_min;
		u16 s_min;
		u16 x_min;
		u16 a_max;
		u16 m_max;
		u16 s_max;
		u16 x_max;
	};
	u16 e[2][4];
} Part_Range;

void
QuicksortWorkflows(Workflow* workflows, uint workflows_len)
{
	if (workflows_len <= 1) return;

	uint j = 0;
	for (uint i = 0; i < workflows_len; ++i)
	{
		if (workflows[i].id <= workflows[workflows_len-1].id)
		{
			Workflow tmp = workflows[j];
			workflows[j] = workflows[i];
			workflows[i] = tmp;

			j += 1;
		}
	}

	QuicksortWorkflows(workflows, j-1);
	QuicksortWorkflows(workflows + j, workflows_len - j);
}

Workflow*
FindWorkflow(Workflow* workflows, uint workflows_len, u16 id)
{
	uint a = 0;
	uint b = workflows_len;

	for (int i = 0; i < 10; ++i)
	{
		uint m = (a + b)/2;

		if (workflows[m].id <= id) a = m;
		else                       b = m;
	}

	ASSERT(workflows[a].id == id);
	return &workflows[a];
}

#define REJECT 0
#define ACCEPT 1
#define BASE_RULE_COUNT 2

void
FindAcceptedRanges(Rule* rules, uint rules_len, Part_Range** accepted_ranges, uint* accepted_ranges_cap, uint* accepted_ranges_len, u16 rule_cur, Part_Range range)
{
	if (rule_cur == ACCEPT)
	{
		for (uint i = 0; i < 4; ++i) ASSERT(range.e[0][i] < range.e[1][i]);

		if (*accepted_ranges_len == *accepted_ranges_cap)
		{
			*accepted_ranges_cap *= 2;
			accepted_ranges = realloc(accepted_ranges, *accepted_ranges_cap*sizeof(Part_Range));
		}

		(*accepted_ranges)[*accepted_ranges_len] = range;
		*accepted_ranges_len += 1;

		uint r = *accepted_ranges_len-1;

		bool should_continue = true;
		while (should_continue)
		{
			should_continue = false;

			for (uint i = 0; i < *accepted_ranges_len; ++i)
			{
				if (i == r) continue;

				bool does_overlap = true;
				for (uint j = 0; j < 4; ++j)
				{
					does_overlap = (does_overlap && ((*accepted_ranges)[r].e[1][j] >= (*accepted_ranges)[i].e[0][j] && (*accepted_ranges)[r].e[0][j] <= (*accepted_ranges)[i].e[1][j]));
				}

				if (does_overlap)
				{
					for (uint k = 0; k < 4; ++k)
					{
						(*accepted_ranges)[i].e[0][k] = MIN((*accepted_ranges)[i].e[0][k], (*accepted_ranges)[r].e[0][k]);
						(*accepted_ranges)[i].e[1][k] = MAX((*accepted_ranges)[i].e[1][k], (*accepted_ranges)[r].e[1][k]);
					}

					(*accepted_ranges)[r]  = (*accepted_ranges)[*accepted_ranges_len-1];
					*accepted_ranges_len -= 1;

					r = (i == *accepted_ranges_len-2 ? *accepted_ranges_len-1 : i);
					should_continue = true;
				}
			}
		}
	}
	else if (rule_cur != REJECT)
	{
		Rule* rule = &rules[rule_cur];

		Part_Range true_range  = range;
		Part_Range false_range = range;

		if (rule->cmp_mask == 0)
		{
			true_range.e[1][rule->var_idx] = MIN(true_range.e[1][rule->var_idx], rule->val - 1);

			false_range.e[0][rule->var_idx] = MAX(false_range.e[0][rule->var_idx], rule->val);
		}
		else
		{
			true_range.e[0][rule->var_idx] = MAX(true_range.e[0][rule->var_idx], rule->val + 1);

			false_range.e[1][rule->var_idx] = MIN(false_range.e[1][rule->var_idx], rule->val);
		}

		if (true_range.e[0][rule->var_idx] <= true_range.e[1][rule->var_idx])
		{
			FindAcceptedRanges(rules, rules_len, accepted_ranges, accepted_ranges_cap, accepted_ranges_len, rule->true_child, true_range);
		}

		if (false_range.e[0][rule->var_idx] <= false_range.e[1][rule->var_idx])
		{
			FindAcceptedRanges(rules, rules_len, accepted_ranges, accepted_ranges_cap, accepted_ranges_len, rule->false_child, false_range);
		}
	}
}

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	static Workflow workflows[1024];
	uint workflows_len = 0;

	workflows[workflows_len++] = (Workflow){
		.id         = 'R' - 'A',
		.first_rule = 0,
	};

	workflows[workflows_len++] = (Workflow){
		.id         = 'A' - 'A',
		.first_rule = 1,
	};

	static Rule rules[ARRAY_SIZE(workflows)*4 + 2];
	uint rules_len = 0;

	rules[rules_len++] = (Rule){
		.var_idx     = 0,
		.cmp_mask    = 0,
		.val         = 0,
		.true_child  = REJECT,
		.false_child = REJECT,
	};

	rules[rules_len++] = (Rule){
		.var_idx     = 0,
		.cmp_mask    = 0,
		.val         = 0,
		.true_child  = ACCEPT,
		.false_child = ACCEPT,
	};

	umm i = 0;
	for (;;)
	{
		u16 wf_id = 0;
		while (i < input.size && input.data[i] != '{')
		{
			wf_id *= 26;
			wf_id += input.data[i] - 'a';

			i += 1;
		}

		ASSERT(i < input.size && input.data[i] == '{');
		i += 1;

		Workflow* wf = &workflows[workflows_len++];
		wf->id = wf_id;

		s16* rule_slot = (s16*)&wf->first_rule;
		for (;;)
		{
			ASSERT(i+1 < input.size);
			if (input.data[i+1] == '<' || input.data[i+1] == '>')
			{
				ASSERT(input.data[i] == 'x' || input.data[i] == 'm' || input.data[i] == 'a' || input.data[i] == 's');

				// NOTE: ASCII -> idx mapping
				// a: (0x61 & 0x1F) >> 3 = (0x01) >> 3 = 0
				// m: (0x6D & 0x1F) >> 3 = (0x0D) >> 3 = 1
				// s: (0x73 & 0x1F) >> 3 = (0x13) >> 3 = 2
				// x: (0x78 & 0x1F) >> 3 = (0x18) >> 3 = 3
				u8 var_idx  = (input.data[i] & 0x1F) >> 3;
				s8 cmp_mask = (input.data[i+1] == '>' ? -1 : 0);
				i += 2;

				s16 val = 0;
				while (i < input.size && (u8)(input.data[i]-0x30) < (u8)10)
				{
					val *= 10;
					val += input.data[i] & 0xF;

					i += 1;
				}

				ASSERT(i < input.size && input.data[i] == ':');
				i += 1;

				u16 id = 0;
				while (i < input.size && input.data[i] != ',')
				{
					id *= 26;
					id += (input.data[i]|0x20) - 'a';

					i += 1;
				}

				ASSERT(i < input.size && input.data[i] == ',');
				i += 1;

				Rule* rule = &rules[rules_len++];
				*rule = (Rule){
					.var_idx    = var_idx,
					.cmp_mask   = cmp_mask,
					.val        = val,
					.true_child = -(s16)id - 1,
				};

				*rule_slot = rules_len-1;
				rule_slot  = &rule->false_child;
			}
			else
			{
				u16 id = 0;
				while (i < input.size && input.data[i] != '}')
				{
					id *= 26;
					id += (input.data[i]|0x20) - 'a';

					i += 1;
				}

				ASSERT(i < input.size && input.data[i] == '}');
				i += 1;

				*rule_slot = -(s16)id - 1;

				break;
			}
		}

		ASSERT(input.data[i] == '\r');
		i += 2;
		if (input.data[i] == '\r')
		{
			i += 2;
			break;
		}
	}

	QuicksortWorkflows(workflows, workflows_len);

	for (int j = 2; j < rules_len; ++j)
	{
		if (rules[j].true_child  < 0) rules[j].true_child  = FindWorkflow(workflows, workflows_len, (u16)-rules[j].true_child - 1)->first_rule;
		if (rules[j].false_child < 0) rules[j].false_child = FindWorkflow(workflows, workflows_len, (u16)-rules[j].false_child - 1)->first_rule;
	}
/*
	for (;;)
	{
		bool made_progress = false;

		for (int j = BASE_RULE_COUNT; j < rules_len; ++j)
		{
			u16 tc = rules[j].true_child;
			u16 fc = rules[j].false_child;

			if (rules[tc].var_idx == rules[j].var_idx)
			{
				if (rules[tc].cmp_mask == rules[j].cmp_mask && (rules[tc].cmp_mask == 0  && rules[tc].val >= rules[j].val ||
							                                          rules[tc].cmp_mask == -1 && rules[tc].val <= rules[j].val))
				{
					rules[j].true_child = rules[tc].true_child;
				}
			}

			if (rules[fc].var_idx == rules[j].var_idx)
			{
				if (rules[fc].cmp_mask != rules[j].cmp_mask && (rules[j].cmp_mask == 0  && rules[fc].val < rules[j].val ||
							                                          rules[j].cmp_mask == -1 && rules[fc].val > rules[j].val))
				{
					rules[j].false_child = rules[fc].true_child;
				}
			}
		}

		for (int j = BASE_RULE_COUNT; j < rules_len; ++j)
		{
			u16 tc = rules[j].true_child;
			u16 fc = rules[j].false_child;

			if (tc >= BASE_RULE_COUNT && rules[tc].true_child == rules[tc].false_child)
			{
				rules[j].true_child = rules[tc].true_child;
				made_progress = true;
			}

			if (fc >= BASE_RULE_COUNT && rules[fc].true_child == rules[fc].false_child)
			{
				rules[j].false_child = rules[fc].true_child;
				made_progress = true;
			}
		}

		if (!made_progress) break;
	}

	u16 live_rules = 2;
	for (int j = 0; j < rules_len; ++j)
	{
		if (rules[j].true_child >= BASE_RULE_COUNT || rules[j].false_child >= BASE_RULE_COUNT)
		{
			live_rules += 1;
		}
	}

	printf("Live rules: %u\n", live_rules);*/

	u16 in_rule = FindWorkflow(workflows, workflows_len, ('i'-'a')*26 + ('n'-'a'))->first_rule;

	uint part1_result = 0;
	while (i < input.size)
	{
		int k = i;
		ASSERT(input.data[i] == '{');
		i += 1;

		s16 vars[4] = {0};

		for (int j = 0; j < 4; ++j)
		{
			ASSERT(i+1 < input.size && input.data[i] == ((u8[4]){'x', 'm', 'a', 's'})[j] && input.data[i+1] == '=');
			u8 var_idx  = (input.data[i] & 0x1F) >> 3;
			i += 2;
			
			while (i < input.size && (u8)(input.data[i]-0x30) < (u8)10)
			{
				vars[var_idx] *= 10;
				vars[var_idx] += input.data[i] & 0xF;

				i += 1;
			}

			ASSERT(i < input.size && (j < 3 && input.data[i] == ',' || j == 3 && input.data[i] == '}'));
			i += 1;
		}

		ASSERT(i < input.size && input.data[i] == '\r' && input.data[i+1] == '\n');
		i += 2;

		u16 rule_cur = in_rule;
		while (rule_cur >= BASE_RULE_COUNT)
		{
			s16 var      = vars[rules[rule_cur].var_idx];
			s16 cmp_mask = rules[rule_cur].cmp_mask;

			if ((var^cmp_mask) < (rules[rule_cur].val^cmp_mask)) rule_cur = rules[rule_cur].true_child;
			else                                                 rule_cur = rules[rule_cur].false_child;
		}

		if (rule_cur == ACCEPT)
		{
			for (int j = 0; j < 4; ++j) part1_result += (u16)vars[j];
		}
	}

	printf("Part 1: %llu\n", part1_result);

	uint accepted_ranges_cap = 65535;
	Part_Range* accepted_ranges = malloc(sizeof(Part_Range)*accepted_ranges_cap);
	uint accepted_ranges_len = 0;

	Part_Range range;
	for (int j = 0; j < 4; ++j)
	{
		range.e[0][j] = 1;
		range.e[1][j] = 4000;
	}

	FindAcceptedRanges(rules, rules_len, &accepted_ranges, &accepted_ranges_cap, &accepted_ranges_len, in_rule, range);

	uint part2_result = 0;

	for (uint j = 0; j < accepted_ranges_len; ++j)
	{
		uint combinations = 1;
		for (uint k = 0; k < 4; ++k)
		{
			combinations *= (accepted_ranges[j].e[1][k] + 1) - accepted_ranges[j].e[0][k];
		}

		part2_result += combinations;
	}

	printf("Part 2: %llu\n", part2_result);

	return 0;
}
