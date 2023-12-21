#include "../aoc.h"

uint
NumArrangements(u64 pattern, uint pattern_length, uint* wildcards, uint wildcard_count, uint* criteria, uint criteria_count)
{
  uint result = 0;

  for (uint j = 0; j < (1ULL<<wildcard_count); ++j)
  {
    u64 test_pattern = pattern;
    for (uint k = 0; k < wildcard_count; ++k)
    {
      test_pattern &= ~(1<<wildcards[k]);
      test_pattern |= ((j>>k)&1) << wildcards[k];
    }

    uint matched_criteria = 0;
    for (uint k = 0; k < pattern_length; ++k)
    {
      if ((test_pattern>>k)&1)
      {
        uint first_idx = k;
        for (; k < pattern_length && ((test_pattern>>k)&1) != 0; ++k);


        if (matched_criteria < criteria_count && criteria[matched_criteria] == k-first_idx)
        {
          ++matched_criteria;
          continue;
        }
        else
        {
          matched_criteria = 0;
          break;
        }
      }
    }

    result += (matched_criteria == criteria_count);
  }

  return result;
}

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;

  uint part1_result = 0;
  uint part2_result = 0;

  for (uint i = 0; i < input.size;)
  {
    uint line_start = i;

    u64 pattern = 0;
    uint wildcards[128];
    uint pattern_length = 0;
    uint wildcard_count = 0;

    for (; input.data[i] != ' '; ++i)
    {
      pattern |= ((input.data[i] == '#') << pattern_length);

      if (input.data[i] == '?')
      {
        ASSERT(wildcard_count < ARRAY_SIZE(wildcards)-1);
        wildcards[wildcard_count++] = pattern_length;
      }

      ++pattern_length;
    }

    i += 1;

    uint criteria[11];
    uint criteria_count = 0;

    for (;;)
    {
      uint num = 0;
      for (; (u8)(input.data[i]-0x30) < (u8)10; ++i) num = num*10 + (input.data[i]&0xF);

      ASSERT(criteria_count < ARRAY_SIZE(criteria));
      criteria[criteria_count++] = num;

      if (i >= input.size || input.data[i] != ',') break;
      else
      {
        ++i;
        continue;
      }
    }

    i += 2;


    uint num_arrangements = NumArrangements(pattern, pattern_length, wildcards, wildcard_count, criteria, criteria_count);

    uint ext_arr = num_arrangements;
    uint ext_arr_prefix = 0;
    uint ext_arr_suffix = 0;
    if (!(pattern>>(pattern_length-1))) // NOTE: Gluing on a wildcard does nothing if the pattern ends with #
    {
      u64 prefix_pattern = pattern<<1;
      uint prefix_wildcards[22];
      prefix_wildcards[0] = 0;
      for (uint j = 0; j < wildcard_count; ++j) prefix_wildcards[j+1] = wildcards[j]+1;

      ext_arr_prefix = NumArrangements(prefix_pattern, pattern_length+1, prefix_wildcards, wildcard_count+1, criteria, criteria_count);

      wildcards[wildcard_count] = pattern_length;
      ext_arr_suffix = NumArrangements(pattern, pattern_length+1, wildcards, wildcard_count+1, criteria, criteria_count);

      //ext_arr = ext_arr_prefix;
      //if (ext_arr_suffix > ext_arr_prefix) ext_arr = ext_arr_suffix;

      //ASSERT(ext_arr + num_arrangements == ext_arr_prefix + ext_arr_suffix);

      //ext_arr = ext_arr_suffix + (ext_arr_prefix - num_arrangements);
    }

    part1_result += num_arrangements;
    //part2_result += ext_arr*ext_arr*ext_arr*ext_arr*num_arrangements;
    
    uint arrs = 0;

    arrs += ext_arr_prefix*ext_arr_prefix*ext_arr_prefix*ext_arr_prefix*num_arrangements;
    arrs += ext_arr_prefix*ext_arr_prefix*ext_arr_prefix*num_arrangements*num_arrangements;
    arrs += ext_arr_prefix*ext_arr_prefix*num_arrangements*num_arrangements*num_arrangements;
    arrs += ext_arr_prefix*num_arrangements*num_arrangements*num_arrangements*num_arrangements;
    arrs += ext_arr_prefix*ext_arr_prefix*ext_arr_prefix*num_arrangements*ext_arr_suffix;
    arrs += ext_arr_prefix*ext_arr_prefix*num_arrangements*num_arrangements*ext_arr_suffix;
    arrs += ext_arr_prefix*ext_arr_prefix*num_arrangements*ext_arr_suffix*num_arrangements;
    arrs += ext_arr_prefix*ext_arr_prefix*num_arrangements*ext_arr_suffix*ext_arr_suffix;

    part2_result += arrs;
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
