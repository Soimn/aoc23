#define SMN_INLINE_IMPLEMENTATION
#include "..\smn.h"

u64
NumArrangements(u8* pattern, uint pattern_len, u64* criteria, uint criteria_len, uint criteria_sum)
{
  u64 arrangements = 0;

  if (criteria_len == 0)
  {
    bool no_thorps = true;
    for (uint i = 0; i < pattern_len && no_thorps; ++i) no_thorps = (pattern[i] != '#');

    arrangements += (no_thorps ? 1 : 0);
  }
  else if (pattern_len >= criteria_sum)
  {
    uint prefix_len = criteria[0];

    for (uint i = 0; i <= pattern_len - criteria_sum; ++i)
    {
      u8* prefix = pattern + i;
      u8* suffix = prefix + prefix_len + 1;

      bool does_match;
      uint suffix_len;
      uint new_criteria_sum;
      if (i < pattern_len - prefix_len)
      {
        does_match       = (prefix[prefix_len] != '#');
        suffix_len       = pattern_len - (i + prefix_len + 1);
        new_criteria_sum = criteria_sum - (criteria[0]+1);
      }
      else
      {
        does_match       = true;
        suffix_len       = 0;
        new_criteria_sum = criteria_sum - criteria[0];
      }

      for (uint j = 0; j < prefix_len && does_match; ++j)
      {
        does_match = (prefix[j] != '.');
      }

      if (does_match)
      {
        arrangements += NumArrangements(suffix, suffix_len, criteria+1, criteria_len-1, new_criteria_sum);
      }

      // NOTE: prevent skipping past #
      if (prefix[0] == '#') break;
    }
  }

  return arrangements;
}

void
OldSolution(String input)
{
  u64 part1_result = 0;
  u64 part2_result = 0;
  for (;;)
  {
    u8 pattern[20*5+4];
    umm pattern_len = 0;
    u64 criteria[6*5];
    umm criteria_len = 0;


    input = String_EatWhitespace(input);
    if (input.size == 0) break;

    umm split = (umm)String_FindFirstChar(input, ' ');

    pattern_len = String_CopyToBuffer(String_FirstN(input, split), pattern, ARRAY_SIZE(pattern));
    ASSERT(pattern_len <= ARRAY_SIZE(pattern)/5);

    for (uint i = 1; i < 5; ++i)
    {
      pattern[i*(pattern_len+1)-1] = '?';
      String_CopyToBuffer(String_FirstN(input, split), pattern + i*(pattern_len+1), pattern_len);
    }

    input = String_EatN(input, split+1);
    umm end = (umm)String_FindFirstChar(input, '\r');

    for (;;)
    {
      input = String_EatU64(input, 10, &criteria[criteria_len++], 0);

      if (input.size == 0 || input.data[0] != ',') break;
      else
      {
        input = String_EatN(input, 1);
        continue;
      }
    }

    for (uint i = 0; i < criteria_len; ++i)
    {
      for (uint j = 1; j < 5; ++j) criteria[j*criteria_len + i] = criteria[i];
    }

    input = String_EatN(input, sizeof("\r\n")-1);

    uint criteria_sum = criteria_len-1;
    for (uint i = 0; i < criteria_len; ++i) criteria_sum += criteria[i];

    part1_result += NumArrangements(pattern, pattern_len, criteria, criteria_len, criteria_sum);
    part2_result += NumArrangements(pattern, pattern_len*5 + 4, criteria, criteria_len*5, criteria_sum*5 + 4);
    //printf("%.*s - %llu\n", (int)pattern_len*5+4, pattern, NumArrangements(pattern, pattern_len*5 + 4, criteria, criteria_len*5, criteria_sum*5 + 4));
    //fflush(stdout);
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);
}

u64
NumArrangementsBitmask(u128 thorp_pattern, u128 dot_pattern, uint pattern_len, u64* criteria, uint criteria_len, uint criteria_sum)
{
  u64 arrangements = 0;

  if      (criteria_len == 0) arrangements = (thorp_pattern == 0);
  else if (pattern_len >= criteria_sum)
  {
    uint prefix_len = criteria[0];

    for (uint i = 0; i <= pattern_len - criteria_sum; ++i)
    {
      u128 thorp_prefix = thorp_pattern >> i;
      u128 dot_prefix   = dot_pattern   >> i;

      uint suffix_len = 0;
      uint new_criteria_sum = criteria_sum - criteria[0];

      if (i < pattern_len - prefix_len)
      {
        suffix_len        = pattern_len - (i + prefix_len + 1);
        new_criteria_sum -= 1;
      }
      
      if (!(((thorp_prefix >> prefix_len)&1) | (dot_prefix << -prefix_len)))
      {
        u128 thorp_suffix = thorp_prefix >> (prefix_len+1);
        u128 dot_suffix   = dot_prefix   >> (prefix_len+1);

        arrangements += NumArrangementsBitmask(thorp_suffix, dot_suffix, suffix_len, criteria+1, criteria_len-1, new_criteria_sum);
      }

      // NOTE: prevent skipping past #
      if (thorp_prefix&1) break;
    }
  }

  return arrangements;
}

u64
NumArrangementsBitmaskSplit64(u64 thorp_pattern, u64 dot_pattern, uint pattern_len,
                              u64* criteria, uint criteria_len, uint criteria_sum)
{
  u64 arrangements = 0;

  if      (criteria_len == 0) arrangements = (thorp_pattern == 0);
  else if (pattern_len >= criteria_sum)
  {
    uint mid_criteria_idx = criteria_len/2;
    uint mid_criteria     = criteria[mid_criteria_idx];

    uint left_criteria_len = mid_criteria_idx;
    uint left_criteria_sum = (left_criteria_len == 0 ? 0 : left_criteria_len-1);

    for (uint i = 0; i < left_criteria_len; ++i) left_criteria_sum += criteria[i];

    uint left_criteria_pad = left_criteria_sum + (left_criteria_len != 0);

    uint right_criteria_len = criteria_len - (mid_criteria_idx + 1);
    uint right_criteria_pad = criteria_sum - (left_criteria_pad + mid_criteria);
    uint right_criteria_sum = right_criteria_pad - (right_criteria_len != 0);

    uint last_idx = pattern_len-(right_criteria_pad + mid_criteria);
    for (uint i = left_criteria_pad; i <= last_idx; ++i)
    {
      u64 attempt_mask = (((u64)1 << (i+mid_criteria)) - 1) & ~(((u64)1 << i) - 1);

      bool no_blocking_dots = ((dot_pattern & attempt_mask) == 0);

      bool no_blocking_thorp_left  = (i == 0 || (thorp_pattern & ((u64)1 << (i-1))) == 0);
      bool no_blocking_thorp_right = ((thorp_pattern & ((u64)1 << (i+mid_criteria))) == 0);

      if (no_blocking_dots && no_blocking_thorp_left && no_blocking_thorp_right)
      {
        uint left_len = (i == 0 ? 0 : i-1);
        u64 left_thorp_pattern = thorp_pattern & (((u64)1 << left_len)-1);
        u64 left_dot_pattern   = dot_pattern   & (((u64)1 << left_len)-1);

        u64 left_arr = (left_thorp_pattern == 0);

        if (left_criteria_len != 0)
        {
          left_arr = NumArrangementsBitmaskSplit64(left_thorp_pattern, left_dot_pattern, left_len,
                                                   criteria, left_criteria_len, left_criteria_sum);
        }
        
        uint past_mid_idx = i+mid_criteria+(right_criteria_len != 0);
        uint right_len = pattern_len - past_mid_idx;
        u64 right_thorp_pattern = thorp_pattern >> past_mid_idx;
        u64 right_dot_pattern   = dot_pattern >> past_mid_idx;

        u64 right_arr = (right_thorp_pattern == 0);

        if (right_criteria_len != 0)
        {
          right_arr = NumArrangementsBitmaskSplit64(right_thorp_pattern, right_dot_pattern, right_len,
                                                    criteria+mid_criteria_idx+1, right_criteria_len,
                                                    right_criteria_sum);
        }

        arrangements += left_arr*right_arr;
      }
    }
  }

  return arrangements;
}

u64
NumArrangementsBitmaskSplit(u128 thorp_pattern, u128 dot_pattern, uint pattern_len,
                            u64* criteria, uint criteria_len, uint criteria_sum)
{
  u64 arrangements = 0;

  if      (criteria_len == 0) arrangements = (thorp_pattern == 0);
  else if (pattern_len >= criteria_sum)
  {
    uint mid_criteria_idx = criteria_len/2;
    uint mid_criteria     = criteria[mid_criteria_idx];

    uint left_criteria_len = mid_criteria_idx;
    uint left_criteria_sum = (left_criteria_len == 0 ? 0 : left_criteria_len-1);

    for (uint i = 0; i < left_criteria_len; ++i) left_criteria_sum += criteria[i];

    uint left_criteria_pad = left_criteria_sum + (left_criteria_len != 0);

    uint right_criteria_len = criteria_len - (mid_criteria_idx + 1);
    uint right_criteria_pad = criteria_sum - (left_criteria_pad + mid_criteria);
    uint right_criteria_sum = right_criteria_pad - (right_criteria_len != 0);

    uint last_idx = pattern_len-(right_criteria_pad + mid_criteria);
    for (uint i = left_criteria_pad; i <= last_idx; ++i)
    {
      u128 attempt_mask = (((u128)1 << (i+mid_criteria)) - 1) & ~(((u128)1 << i) - 1);

      bool no_blocking_dots = ((dot_pattern & attempt_mask) == 0);

      bool no_blocking_thorp_left  = (i == 0 || (thorp_pattern & ((u128)1 << (i-1))) == 0);
      bool no_blocking_thorp_right = ((thorp_pattern & ((u128)1 << (i+mid_criteria))) == 0);

      if (no_blocking_dots && no_blocking_thorp_left && no_blocking_thorp_right)
      {
        uint left_len = (i == 0 ? 0 : i-1);
        u128 left_thorp_pattern = thorp_pattern & (((u128)1 << left_len)-1);
        u128 left_dot_pattern   = dot_pattern   & (((u128)1 << left_len)-1);

        u64 left_arr = (left_thorp_pattern == 0);

        if (left_criteria_len != 0)
        {
          if (left_len < 64)
          {
            left_arr = NumArrangementsBitmaskSplit64((u64)left_thorp_pattern, (u64)left_dot_pattern, left_len,
                                                     criteria, left_criteria_len, left_criteria_sum);
          }
          else
          {
            left_arr = NumArrangementsBitmaskSplit(left_thorp_pattern, left_dot_pattern, left_len,
                                                   criteria, left_criteria_len, left_criteria_sum);
          }
        }
        
        uint past_mid_idx = i+mid_criteria+(right_criteria_len != 0);
        uint right_len = pattern_len - past_mid_idx;
        u128 right_thorp_pattern = thorp_pattern >> past_mid_idx;
        u128 right_dot_pattern   = dot_pattern >> past_mid_idx;

        u64 right_arr = (right_thorp_pattern == 0);

        if (right_criteria_len != 0)
        {
          if (right_len < 64)
          {
            right_arr = NumArrangementsBitmaskSplit64((u64)right_thorp_pattern, (u64)right_dot_pattern, right_len,
                                                      criteria+mid_criteria_idx+1, right_criteria_len,
                                                      right_criteria_sum);
          }
          else
          {
            right_arr = NumArrangementsBitmaskSplit(right_thorp_pattern, right_dot_pattern, right_len,
                                                    criteria+mid_criteria_idx+1, right_criteria_len,
                                                    right_criteria_sum);
          }
        }

        arrangements += left_arr*right_arr;
      }
    }
  }

  return arrangements;
}

typedef struct State
{
  u64 thorp[2];
  u64 dots[2];
  u64 criteria;
  u8 pattern_len;
  u8 criteria_len;
} State;

typedef SBMap(State, u64) State_Memo;

u64
State_Hash(void* state)
{
  return String_FNV((String){ .data = state, .size = 42 });
}

bool
State_Match(void* a, void* b)
{
  return String_Match((String){ .data = a, .size = 42 }, (String){ .data = b, .size = 42 });
}

u64
NumArrangementsBitmaskDP(State_Memo* memo, u128 thorp_pattern, u128 dot_pattern, uint pattern_len, u64* criteria, uint criteria_len, uint criteria_sum)
{
  u64 arrangements = 0;

  if      (criteria_len == 0) arrangements = (thorp_pattern == 0);
  else if (pattern_len >= criteria_sum)
  {
    State state = {
      .thorp[0] = (u64)(thorp_pattern),
      .thorp[1] = (u64)(thorp_pattern>>64),
      .dots[0] = (u64)(dot_pattern),
      .dots[1] = (u64)(dot_pattern>>64),
      .pattern_len = pattern_len,
      .criteria = (u64)criteria,
      .criteria_len = (u32)criteria_len
    };
    u64 memoed_arr;
    if (SBMap_Get(memo, state, &memoed_arr)) arrangements = memoed_arr;
    else
    {
      uint prefix_len = criteria[0];

      for (uint i = 0; i <= pattern_len - criteria_sum; ++i)
      {
        u128 thorp_prefix = thorp_pattern >> i;
        u128 dot_prefix   = dot_pattern   >> i;

        uint suffix_len = 0;
        uint new_criteria_sum = criteria_sum - criteria[0];

        if (i < pattern_len - prefix_len)
        {
          suffix_len        = pattern_len - (i + prefix_len + 1);
          new_criteria_sum -= 1;
        }
        
        if (!(((thorp_prefix >> prefix_len)&1) | (dot_prefix << -prefix_len)))
        {
          u128 thorp_suffix = thorp_prefix >> (prefix_len+1);
          u128 dot_suffix   = dot_prefix   >> (prefix_len+1);

          arrangements += NumArrangementsBitmask(thorp_suffix, dot_suffix, suffix_len, criteria+1, criteria_len-1, new_criteria_sum);
        }

        // NOTE: prevent skipping past #
        if (thorp_prefix&1) break;
      }

      SBMap_Put(memo, state, arrangements);
    }
  }

  return arrangements;
}

int
main(int argc, char** argv)
{
  ASSERT(argc == 2);

  String input = {0};
  {
    FILE* file;
    errno_t open_err = fopen_s(&file, argv[1], "rb");
    ASSERT(open_err == 0);

    fseek(file, 0, SEEK_END);
    umm file_size = ftell(file);
    rewind(file);

    input.data = malloc(file_size);
    input.size = file_size;

    umm bytes_read = fread(input.data, 1, file_size, file);

    ASSERT(bytes_read == file_size);

    fclose(file);
  }

  State_Memo memo = 0;
  SBMap_Create(&memo, State_Hash, State_Match);

  u64 part1_result = 0;
  u64 part2_result = 0;
  for (uint r = 0;; ++r)
  {
    u128 thorp_pattern = 0;
    u128 dot_pattern   = 0;
    umm pattern_len    = 0;
    u64 criteria[6*5];
    umm criteria_len = 0;


    input = String_EatWhitespace(input);
    if (input.size == 0) break;

    pattern_len = (umm)String_FindFirstChar(input, ' ');

    for (uint i = 0; i < pattern_len; ++i)
    {
      for (uint j = 0; j < 5; ++j)
      {
        thorp_pattern |= ((u128)(input.data[i] == '#') << (j*(pattern_len+1) + i));
        dot_pattern   |= ((u128)(input.data[i] == '.') << (j*(pattern_len+1) + i));
      }
    }

    u8* a = input.data;
    input = String_EatN(input, pattern_len+1);
    umm end = (umm)String_FindFirstChar(input, '\r');

    for (;;)
    {
      input = String_EatU64(input, 10, &criteria[criteria_len++], 0);

      if (input.size == 0 || input.data[0] != ',') break;
      else
      {
        input = String_EatN(input, 1);
        continue;
      }
    }

    for (uint i = 0; i < criteria_len; ++i)
    {
      for (uint j = 1; j < 5; ++j) criteria[j*criteria_len + i] = criteria[i];
    }

    input = String_EatN(input, sizeof("\r\n")-1);

    uint criteria_sum = criteria_len-1;
    for (uint i = 0; i < criteria_len; ++i) criteria_sum += criteria[i];

    u128 thorp_pattern_trunc = thorp_pattern & (((u128)1 << (pattern_len+1)) - 1);
    u128 dot_pattern_trunc   = dot_pattern   & (((u128)1 << (pattern_len+1)) - 1);

    part1_result += NumArrangementsBitmask(thorp_pattern_trunc, dot_pattern_trunc, pattern_len, criteria, criteria_len, criteria_sum);
    //part2_result += NumArrangementsBitmask(thorp_pattern, dot_pattern, pattern_len*5+4, criteria, criteria_len*5, criteria_sum*5+4);

#if 1
    part2_result += NumArrangementsBitmaskSplit(thorp_pattern, dot_pattern, pattern_len*5+4, criteria, criteria_len*5, criteria_sum*5+4);
#else
    if (r % 10 == 0) printf("\r%u%%", r/10);
    SBMap_Clr(&memo);
    part2_result += NumArrangementsBitmaskDP(&memo, thorp_pattern, dot_pattern, pattern_len*5+4, criteria, criteria_len*5, criteria_sum*5+4);
#endif
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

}
