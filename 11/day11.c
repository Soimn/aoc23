#include "../aoc.h"

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;

  V2S galaxies[441];
  uint galaxy_count = 0;

  u64 vert_occupancy[5] = {0};
  uint horiz_adj[140]   = {0};
  uint vert_adj[140]    = {0};

  uint map_width = 0;
  while (input.data[map_width] != '\r') ++map_width;
  
  for (uint j = 0, inflation = 0; j*(map_width+2) < input.size; ++j)
  {
    uint prev_galaxy_count = galaxy_count;
    for (uint i = 0; i < map_width; ++i)
    {
      if (input.data[j*(map_width+2) + i] == '#')
      {
        ASSERT(galaxy_count < ARRAY_SIZE(galaxies));
        galaxies[galaxy_count++] = V2S((sint)i, (sint)j);
        vert_occupancy[i>>6] |= (1ULL << (i&63));
      }
    }

    vert_adj[j] = (j > 0 ? vert_adj[j-1] : 0) + (galaxy_count == prev_galaxy_count);
  }

  horiz_adj[0] = !(vert_occupancy[0]&1);
  for (uint i = 1; i < map_width; ++i) horiz_adj[i] = horiz_adj[i-1] + !(vert_occupancy[i>>6]&(1ULL<<(i&63)));

  uint part1_result = 0;
  for (uint i = 0; i < galaxy_count; ++i)
  {
    V2S gi = V2S_Add(galaxies[i], V2S(horiz_adj[galaxies[i].x], vert_adj[galaxies[i].y]));

    for (uint j = i+1; j < galaxy_count; ++j)
    {
      V2S gj = V2S_Add(galaxies[j], V2S(horiz_adj[galaxies[j].x], vert_adj[galaxies[j].y]));
      part1_result += (uint)V2S_ManhattanLength(V2S_Sub(gi, gj));
    }
  }

  uint part2_result = 0;
  sint scale = 1000000-1;
  for (uint i = 0; i < galaxy_count; ++i)
  {
    V2S gi = V2S_Add(galaxies[i], V2S(horiz_adj[galaxies[i].x]*scale, vert_adj[galaxies[i].y]*scale));

    for (uint j = i+1; j < galaxy_count; ++j)
    {
      V2S gj = V2S_Add(galaxies[j], V2S(horiz_adj[galaxies[j].x]*scale, vert_adj[galaxies[j].y]*scale));
      part2_result += (uint)V2S_ManhattanLength(V2S_Sub(gi, gj));
    }
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
