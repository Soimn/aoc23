#include "../aoc.h"

uint
FindReflectionIndex(u32* pattern, uint pattern_height)
{
  uint i = 1;
  for (; i < pattern_height; ++i)
  {
    bool did_match = true;
    for (uint j = 0; j < (i <= pattern_height/2 ? i : pattern_height - i); ++j)
    {
      uint li = i-1 - j;
      uint ri = i   + j;

      did_match &= (pattern[li] == pattern[ri]);
    }

    if (did_match) break;
  }

  return i;
}

uint
FindSmudgedReflectionIndex(u32* pattern, uint pattern_height)
{
  uint result = pattern_height;

  for (uint i = 1; i < pattern_height; ++i)
  {
    uint reflected_rows = (i <= pattern_height/2 ? i : pattern_height - i);

    uint matching_rows = 0;
    u32 diff           = 0;
    for (uint j = 0; j < reflected_rows; ++j)
    {
      uint li = i-1 - j;
      uint ri = i   + j;

      if (pattern[li] == pattern[ri]) matching_rows += 1;
      else                            diff           = pattern[li]^pattern[ri];
    }

    if (matching_rows == reflected_rows-1 && __popcnt(diff) == 1)
    {
      result = i;
      break;
    }
  }

  return result;
}

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;

  uint hori_refl = 0;
  uint vert_refl = 0;

  uint smudged_hori_refl = 0;
  uint smudged_vert_refl = 0;

  for (uint i = 0; i < input.size;)
  {
    u32 pattern[17]                             = {0};
    u32 transposed_pattern[ARRAY_SIZE(pattern)] = {0};

    uint pattern_width  = 0;
    uint pattern_height = 0;

    for (; input.data[i + pattern_width] != '\r'; ++pattern_width);

    for (; i < input.size && input.data[i] != '\r'; ++pattern_height)
    {
      for (uint j = 0; j < pattern_width; ++j, ++i)
      {
        pattern[pattern_height] |= ((u32)(input.data[i] == '#') << j);
      }

      i += 2;
    }

    i += 2;

    for (uint j = 0; j < ARRAY_SIZE(pattern); ++j)
    {
      for (uint k = 0; k < ARRAY_SIZE(pattern); ++k)
      {
        transposed_pattern[j] |= ((pattern[k]>>j)&1) << k;
      }
    }

    uint nidx = FindReflectionIndex(pattern, pattern_height);
    uint tidx = FindReflectionIndex(transposed_pattern, pattern_width);

    ASSERT(nidx > 0 && tidx > 0);
    ASSERT(nidx < pattern_height || tidx < pattern_width);

    if (nidx < pattern_height) hori_refl += nidx;
    else                       vert_refl += tidx;

    uint snidx = FindSmudgedReflectionIndex(pattern, pattern_height);
    uint stidx = FindSmudgedReflectionIndex(transposed_pattern, pattern_width);

    ASSERT(snidx > 0 && stidx > 0);
    ASSERT(snidx < pattern_height || stidx < pattern_width);

    if (snidx < pattern_height) smudged_hori_refl += snidx;
    else                        smudged_vert_refl += stidx;
  }

  uint part1_result = hori_refl*100 + vert_refl;
  uint part2_result = smudged_hori_refl*100 + smudged_vert_refl;

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
