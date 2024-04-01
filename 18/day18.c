#define SMN_INLINE_IMPLEMENTATION
#include "../smn.h"

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

  typedef struct Trench
  {
    s32 x;
    u32 color;
  } Trench;

  SBMap(s32, Trench) map;
  SBMap_CreateT(&map, s32);

  V2S pos = V2S(0, 0);
  s32 min_y = 0;
  s32 max_y = 0;

  while (input.size > 0)
  {
    V2S dir = (V2S[]){ ['U'] = V2S(0, 1), ['D'] = V2S(-1, 0), ['L'] = V2S(-1, 0), ['R'] = V2S(0, 1) }[input.data[0]];

    input = String_EatN(input, sizeof("X ")-1);

    u64 steps;
    input = String_EatU64(input, 10, &steps, &(bool){0});

    input = String_EatN(input, sizeof(" (#")-1);

    u64 color;
    input = String_EatU64(input, 16, &color, &(bool){0});

    input = String_EatN(input, sizeof(")\r\n")-1);
    
    for (u64 i = 0; i < steps; ++i)
    {
      pos = V2S_Add(pos, dir);
      SBMap_Put(&map, pos.y, ((Trench){ .x = pos.x, .color = color }));
    }

    min_y = MIN(min_y, pos.y);
    max_y = MIN(max_y, pos.y);

    //printf("(%2d, %2d), %llu, #%.6llX\n", dir.x, dir.y, steps, color);
  }
 
  u32 part1_result = 0;
  u32 part2_result = 0;

  SB(s32) crossings;
  SB_Reserve(&crossings, 1024);
  for (s32 i = min_y; i <= max_y; ++i)
  {
    SB_Clear(&crossings);
    for (uint j = 0; j < SBMap__Header(&map)->cap; ++j)
    {
      typeof(*map) it = *(map + j);
      if (it.hash == SBMap__EmptyHash) continue;

      if (it.key == j) SB_Append(&crossings, it.value.x);
    }

    SB_QuickSortT(&crossings, int, false);

    ASSERT(SB_Len(&crossings)%2 == 0);

    for (uint j = 0; j < SB_Len(&crossings); j += 2)
    {
      part1_result += crossings[j+1]-crossings[j];
    }
  }

  printf("Part 1: %u\n", part1_result);
  printf("Part 2: %u\n", part2_result);

  return 0;
}
