#include "../aoc.h"

void
PrintMap(u8 map[100][100], uint width, uint height)
{
  for (uint j = 0; j < height; ++j)
  {
    for (uint i = 0; i < width; ++i)
    {
      printf("%c", map[j][i]);
    }

    printf("\n");
  }
}

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;

  uint load               = 0;
  uint blocked_lanes[100] = {0};

  uint height = 0;
  for (uint i = 0; i < input.size; ++i) height += (input.data[i] == '\r');

  for (uint i = 0, j = 0, k = 0; i < input.size; ++i)
  {
    if (input.data[i] == '\r')
    {
      j  = 0;
      k += 1;
      i += 1;
    }
    else
    {
      if      (input.data[i] == '#') blocked_lanes[j] = k+1;
      else if (input.data[i] == 'O')
      {
        load += (height - blocked_lanes[j]);
        blocked_lanes[j] += 1;
      }

      ++j;
    }
  }

  uint part1_result = load;
  uint part2_result = 0;

  u8 map[100][100] = {0};

  uint width = 0;
  for (; input.data[width] != '\r'; ++width);

  for (uint j = 0; j < height; ++j)
  {
    for (uint i = 0; i < width; ++i)
    {
      map[j][i] = input.data[j*(width+2) + i];
    }
  }

  for (uint kkk = 0; kkk <= 1000000000/4; ++kkk)
  {

    { // Tilt north
      uint blocked_lanes[100] = {0};

      for (uint j = 0; j < height; ++j)
      {
        for (uint i = 0; i < width; ++i)
        {
          if      (map[j][i] == '#') blocked_lanes[i] = j+1;
          else if (map[j][i] == 'O')
          {
            map[j][i] = '.';
            map[blocked_lanes[i]][i] = 'O';
            blocked_lanes[i] += 1;
          }
        }
      }
    }

    { // Tilt west
      uint blocked_lanes[100] = {0};

      for (uint i = 0; i < width; ++i)
      {
        for (uint j = 0; j < height; ++j)
        {
          if      (map[j][i] == '#') blocked_lanes[j] = i+1;
          else if (map[j][i] == 'O')
          {
            map[j][i] = '.';
            map[j][blocked_lanes[j]] = 'O';
            blocked_lanes[j] += 1;
          }
        }
      }
    }

    { // Tilt south
      uint blocked_lanes[100] = {0};

      for (uint j = height-1; j < height; --j)
      {
        for (uint i = 0; i < width; ++i)
        {
          if      (map[j][i] == '#') blocked_lanes[i] = height-j;
          else if (map[j][i] == 'O')
          {
            map[j][i] = '.';
            map[(height-1) - blocked_lanes[i]][i] = 'O';
            blocked_lanes[i] += 1;
          }
        }
      }
    }

    { // Tilt east
      uint blocked_lanes[100] = {0};

      for (uint i = width-1; i < width; --i)
      {
        for (uint j = 0; j < height; ++j)
        {
          if      (map[j][i] == '#') blocked_lanes[j] = width-i;
          else if (map[j][i] == 'O')
          {
            map[j][i] = '.';
            map[j][(width-1) - blocked_lanes[j]] = 'O';
            blocked_lanes[j] += 1;
          }
        }
      }
    }

    uint load = 0;
    for (uint j = 0; j < height; ++j)
    {
      for (uint i = 0; i < width; ++i)
      {
        load += (map[j][i] == 'O' ? height-j : 0);
      }
    }

    printf("%llu %llu,\n", kkk, load);
  }

  part2_result = 0;

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
