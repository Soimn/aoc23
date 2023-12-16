#include "../aoc.h"

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;

  uint part1_result = 0;

  for (uint i = 0; i < input.size;)
  {
    uint num_arrangements = 0;

    u32 pattern = 0;
    uint wildcards[21];
    uint pattern_length = 0;
    uint wildcard_count = 0;

    for (; input.data[i] != ' '; ++i)
    {
      pattern |= ((input.data[i] == '#') << pattern_length);

      if (input.data[i] == '?') wildcards[wildcard_count++] = pattern_length;

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

    for (uint j = 0; j < (1ULL<<wildcard_count); ++j)
    {
      u32 test_pattern = pattern;
      for (uint k = 0; k < wildcard_count; ++k)
      {
        test_pattern &= ~(1<<wildcards[k]);
        test_pattern |= ((j>>k)&1) << wildcards[k];
      }

      uint matched_criteria = 0;
      for (uint k = 0; k < 32; ++k)
      {
        if ((test_pattern>>k)&1)
        {
          uint first_idx = k;
          for (; k < 32 && ((test_pattern>>k)&1) != 0; ++k);


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

      if (matched_criteria == criteria_count)
      {
        ++num_arrangements;
        //for (uint k = 0; k < pattern_length; ++k) printf("%c", (((test_pattern>>k)&1) ? '#':'.'));
        //printf("\n");
      }
    }

    //printf("%llu\n", num_arrangements);
    part1_result += num_arrangements;
  }

  printf("Part 1: %llu\n", part1_result);

  return 0;
}
