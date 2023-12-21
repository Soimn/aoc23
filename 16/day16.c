#include "../aoc.h"

typedef enum Tile_Kind
{
  Tile_Empty     = '.',
  Tile_PosMirror = '\\',
  Tile_NegMirror = '/',
  Tile_HSplitter = '|',
  Tile_VSplitter = '-',
} Tile_Kind;

typedef struct Tile
{
  Tile_Kind kind;
  bool is_energized;
  bool is_probed;
} Tile;

Tile Map[110][110];
uint MapSize;

bool
IsInBounds(V2S p)
{
  return ((uint)p.x < MapSize && (uint)p.y < MapSize);
}

void
ProbeBeam(V2S pos, V2S dir)
{
  for (; IsInBounds(pos); pos = V2S_Add(pos, dir))
  {
    Tile* tile = &Map[pos.y][pos.x];

    tile->is_energized = true;

    Tile_Kind right_splitter = Tile_HSplitter;
    Tile_Kind wrong_splitter = Tile_VSplitter;
    if (dir.x == 0)
    {
      right_splitter = Tile_VSplitter;
      wrong_splitter = Tile_HSplitter;
    }

    if (tile->kind == right_splitter)
    {
      if (tile->is_probed) break;
      else
      {
        tile->is_probed = true;

        dir.x ^= (dir.y ^= (dir.x ^= dir.y));

        V2S neg_dir = V2S_Neg(dir);
        ProbeBeam(V2S_Add(pos, neg_dir), neg_dir);
      }
    }
    else if (tile->kind == Tile_PosMirror)
    {
      dir.x ^= (dir.y ^= (dir.x ^= dir.y));
    }
    else if (tile->kind == Tile_NegMirror)
    {
      dir.x ^= (dir.y ^= (dir.x ^= dir.y));
      dir = V2S_Neg(dir);
    }
    else continue;
  }
}

int
main(int argc, char** argv)
{
  String input;
  if (!ReadInput(argc, argv, &input)) return 0;
  
  
  for (; input.data[MapSize] != '\r'; ++MapSize);

  for (uint j = 0; j < MapSize; ++j)
  {
    for (uint i = 0; i < MapSize; ++i)
    {
      Map[j][i] = (Tile){
        .kind         = input.data[j*(MapSize+2) + i],
        .is_energized = false,
        .is_probed    = false,
      };
    }
  }

  ProbeBeam(V2S(0, 0), V2S(1, 0));

  uint part1_result = 0;
  for (uint j = 0; j < MapSize; ++j)
  {
    for (uint i = 0; i < MapSize; ++i)
    {
      part1_result += Map[j][i].is_energized;
      Map[j][i].is_energized = false;
      Map[j][i].is_probed    = false;
    }
  }

  uint part2_result = 0;

  for (uint k = 0; k < 4*MapSize; ++k)
  {
    V2S pos = {0};
    V2S dir = {0};

    bool vert_round = ((k/MapSize)%2 == 0);

    pos.e[ vert_round] = k % MapSize;
    dir.e[!vert_round] = 1 - 2*(sint)(k > 2*MapSize);

    ProbeBeam(pos, dir);

    uint energy = 0;
    for (uint j = 0; j < MapSize; ++j)
    {
      for (uint i = 0; i < MapSize; ++i)
      {
        energy += Map[j][i].is_energized;
        Map[j][i].is_energized = false;
        Map[j][i].is_probed    = false;
      }
    }

    if (energy > part2_result) part2_result = energy;
  }

  printf("Part 1: %llu\n", part1_result);
  printf("Part 2: %llu\n", part2_result);

  return 0;
}
