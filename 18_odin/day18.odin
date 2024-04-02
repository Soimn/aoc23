package main;

import "core:fmt";
import "core:strings";
import "core:strconv";

TestInput :: string(#load("test.txt"));
Input     :: string(#load("input.txt"));

Crossing_Kind :: enum
{
  None,
  I,
  L,
  F,
  J,
  T,
}

DirToCrossingKind := [3][3]Crossing_Kind{
  {.J,    .None, .L   },
  {.None, .None, .None},
  {.T,    .None, .F   },
}

Crossing :: struct
{
  kind: Crossing_Kind,
  x: int,
  color: int,
}

ParseDir :: proc(c: u8) -> [2]int
{
  dir: [2]int;
  switch c
  {
    case 'U': dir = [2]int{ 1,  0};
    case 'D': dir = [2]int{-1,  0};
    case 'L': dir = [2]int{ 0, -1};
    case 'R': dir = [2]int{ 0,  1};
  }

  return dir;
}

main :: proc()
{
  input := Input;

  crossings := make(map[int][dynamic]Crossing);
  pos := [2]int{0, 0};

  lines := strings.split(input, "\r\n");
  lines = lines[:len(lines)-1]
  for line, i in lines
  {
    parts := strings.split(line, " ");

    dir := ParseDir(parts[0][0]);
    steps, _ := strconv.parse_int(parts[1]);
    color, _ := strconv.parse_int(parts[2][2:len(parts[2])-1], base=16);

    prev_dir := ParseDir(lines[(len(lines) + i-1)%len(lines)][0]);
    next_dir := ParseDir(lines[(i+1)%len(lines)][0]);

    if dir.y == 0 do pos += steps*dir;
    else
    {
      for j in 0..<steps
      {
        kind: Crossing_Kind;
        if      j == 0       do kind = DirToCrossingKind[dir.y+1][prev_dir.x+1];
        else if j == steps-1 do kind = DirToCrossingKind[dir.y+1][next_dir.x+1];
        else                 do kind = .I;

        if pos.y not_in crossings do crossings[pos.y] = make([dynamic]Crossing);

        append(&crossings[pos.y], Crossing{ x = pos.x, color = color, kind = kind });
      }
    }
  }

  for y in crossings
  {
  }
}


