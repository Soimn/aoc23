#include "../aoc.h"

u8
Hash(String s)
{
  u8 result = 0;

  for (umm i = 0; i < s.size; ++i)
  {
    result += s.data[i];
    result *= 17;
  }

  return result;
}

typedef struct Hashmap_Entry
{
  String key;
  u8 value;
} Hashmap_Entry;

Hashmap_Entry Hashmap[256][37] = {0};
uint HashmapBlockSizes[256]    = {0};

void
Quicksort(uint* arr, uint size)
{
  if (size <= 1) return;

  uint pivot  = arr[size-1];
  uint cursor = 0;
  for (uint i = 0; i < size; ++i)
  {
    if (arr[i] >= pivot)
    {
      uint tmp = arr[cursor];
      arr[cursor] = arr[i];
      arr[i]      = tmp;
      ++cursor;
    }
  }

  Quicksort(arr, cursor-1);
  Quicksort(arr + cursor, size - cursor);
}

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;
  
  ASSERT(input.data[input.size-2] == '\r' && input.data[input.size-1] == '\n');
  input.size -= 2;

  uint part1_result = 0;

  uint counts[256] = {0};

  for (umm i = 0; i < input.size;)
  {
    String s = { .data = input.data + i, .size = 0 };

    for (; i < input.size && input.data[i] != ','; ++i) ++s.size;
    ++i;

    counts[Hash(s)] += 1;
    part1_result += Hash(s);
  }

  Quicksort(counts, ARRAY_SIZE(counts));
  //printf("chungus %llu, %llu\n", counts[0], counts[1]);

  for (umm i = 0; i < input.size;)
  {
    String s = { .data = input.data + i, .size = 0 };

    for (; i < input.size && input.data[i] != ','; ++i) ++s.size;
    ++i;

    if (s.data[s.size-1] == '-')
    {
      s.size -= 1;
      u8 hash = Hash(s);

      Hashmap_Entry* entries = Hashmap[hash];

      sint removal_idx = -1;
      for (uint j = 0; j < HashmapBlockSizes[hash]; ++j)
      {
        if (String_Match(s, entries[j].key))
        {
          removal_idx = (sint)j;
          break;
        }
      }

      if (removal_idx != -1)
      {
        for (uint j = (uint)removal_idx + 1; j < HashmapBlockSizes[hash]; ++j)
        {
          entries[j-1] = entries[j];
        }

        HashmapBlockSizes[hash] -= 1;
      }
    }
    else
    {
      ASSERT(s.data[s.size-2] == '=' && (u8)(s.data[s.size-1] - 0x30) < (u8)10);
      
      u8 lens = s.data[s.size-1]&0xF;
      s.size -= 2;

      u8 hash = Hash(s);

      uint insertion_idx = 0;
      for (; insertion_idx < HashmapBlockSizes[hash]; ++insertion_idx)
      {
        if (String_Match(s, Hashmap[hash][insertion_idx].key))
        {
          break;
        }
      }

      Hashmap[hash][insertion_idx] = (Hashmap_Entry){
        .key   = s,
        .value = lens,
      };

      if (insertion_idx == HashmapBlockSizes[hash]) HashmapBlockSizes[hash] += 1;
    }
  }

  uint part2_result = 0;
  for (uint i = 0; i < ARRAY_SIZE(Hashmap); ++i)
  {
    for (uint j = 0; j < HashmapBlockSizes[i]; ++j)
    {
      part2_result += (i+1)*(j+1)*Hashmap[i][j].value;
    }
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
