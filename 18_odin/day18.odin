package main;

import "core:fmt"
import "core:strings"
import "core:strconv"
import "core:slice"

TestInput :: string(#load("test.txt"));
Input     :: string(#load("input.txt"));

Crossing_Kind :: enum
{
	I,
	T,
	F,
	L,
	J,
}

Crossing :: struct
{
	kind: Crossing_Kind,
	x: int,
}

DirFromChar :: proc(c: u8) -> (result: [2]int, idx: int)
{
	switch c
	{
		case 'U': result = [2]int{ 0,  1}; idx = 0;
		case 'D': result = [2]int{ 0, -1}; idx = 2;
		case 'L': result = [2]int{-1,  0}; idx = 1;
		case 'R': result = [2]int{ 1,  0}; idx = 3;
	}
	return;
}

CrossingKindFromIdxs :: proc(prev_idx, curr_idx: int) -> Crossing_Kind
{
	lut := [8]Crossing_Kind{
		.T,
		.F,

		.L,
		.F,

		.J,
		.L,

		.J,
		.T,
	};
/*
	T: 0,7
	F: 1,3
	L: 2,5
	J: 4,6
*/

	return lut[prev_idx*2 + curr_idx/2];
}

Part1 :: proc(lines: []string)
{
	pos := [2]int{0, 0};
	ys := [2][dynamic][dynamic]Crossing{make([dynamic][dynamic]Crossing), make([dynamic][dynamic]Crossing)};

	add_crossing := proc(ys: ^[2][dynamic][dynamic]Crossing, pos: [2]int, kind: Crossing_Kind)
	{
		y_sgn, y_abs := 1, pos.y;
		if pos.y < 0 do y_sgn, y_abs = 0, abs(pos.y)-1;

		if len(ys^[y_sgn]) <= y_abs
		{
			assert(len(ys^[y_sgn]) == y_abs);
			append(&ys^[y_sgn], make([dynamic]Crossing));
		}

		append(&ys^[y_sgn][y_abs], Crossing{ kind = kind, x = pos.x });
	};

	for line, i in lines
	{
		prev_dir, prev_idx := DirFromChar(lines[(len(lines) + i-1)%len(lines)][0]);
		curr_dir, curr_idx := DirFromChar(lines[i][0]);

		parts := strings.split(line[2:], " ");

		steps, _ := strconv.parse_int(parts[0], base=10);
		color, _ := strconv.parse_int(parts[1][2:len(parts[1])-1], base=16);

		add_crossing(&ys, pos, CrossingKindFromIdxs(prev_idx, curr_idx));

		if curr_dir.x != 0 do pos += curr_dir*steps;
		else
		{
			pos += curr_dir;
			for j in 0..<steps-1
			{
				add_crossing(&ys, pos, .I);
				pos += curr_dir;
			}
		}
	}

	part1_result := 0;
	for i in 0..<2
	{
		for xs in ys[i]
		{
			slice.sort_by(xs[:], proc(a, b: Crossing) -> bool { return (a.x < b.x); });

			j := 0;
			for j < len(xs)
			{
				start_j := j;
				j += 1;

				assert(j < len(xs));
				if (xs[start_j].kind == .L && xs[j].kind == .T) || (xs[start_j].kind == .F && xs[j].kind == .J)
				{
					j += 1;
				}

				for j+1 < len(xs) && ((xs[j].kind == .L && xs[j+1].kind == .J) || (xs[j].kind == .F && xs[j+1].kind == .T))
				{
					j += 2;
				}

				assert(j < len(xs));
				if j+1 < len(xs) && ((xs[j].kind == .L && xs[j+1].kind == .T) || (xs[j].kind == .F && xs[j+1].kind == .J))
				{
					j += 1;
				}

				part1_result += (xs[j].x+1) - xs[start_j].x;
				j += 1;
			}
		}
	}

	fmt.println("Part 1:", part1_result);
}

Part2 :: proc(lines: []string)
{
	pos := [2]int{0, 0};
	ys := [2][dynamic][dynamic]Crossing{make([dynamic][dynamic]Crossing), make([dynamic][dynamic]Crossing)};

	add_crossing := proc(ys: ^[2][dynamic][dynamic]Crossing, pos: [2]int, kind: Crossing_Kind)
	{
		y_sgn, y_abs := 1, pos.y;
		if pos.y < 0 do y_sgn, y_abs = 0, abs(pos.y)-1;

		if len(ys^[y_sgn]) <= y_abs
		{
			assert(len(ys^[y_sgn]) == y_abs);
			append(&ys^[y_sgn], make([dynamic]Crossing));
		}

		append(&ys^[y_sgn][y_abs], Crossing{ kind = kind, x = pos.x });
	};

	for line, i in lines
	{
		prev_line  := lines[(len(lines) + i-1)%len(lines)];
		prev_color, _ := strconv.parse_int(prev_line[len(prev_line)-7:len(prev_line)-1], base=16);
		curr_color, _ := strconv.parse_int(line[len(line)-7:len(line)-1], base=16);

		prev_idx := 3-(prev_color & 0xF);
		steps, curr_idx := curr_color >> 4, 3-(curr_color & 0xF);

		curr_dir: [2]int;
		if      curr_idx == 0 do curr_dir = [2]int{ 0,  1};
		else if curr_idx == 1 do curr_dir = [2]int{-1,  0};
		else if curr_idx == 2 do curr_dir = [2]int{ 0, -1};
		else if curr_idx == 3 do curr_dir = [2]int{ 1,  0};

		add_crossing(&ys, pos, CrossingKindFromIdxs(prev_idx, curr_idx));

		if curr_dir.x != 0 do pos += curr_dir*steps;
		else
		{
			pos += curr_dir;
			for j in 0..<steps-1
			{
				add_crossing(&ys, pos, .I);
				pos += curr_dir;
			}
		}
	}

	part1_result := 0;
	for i in 0..<2
	{
		for xs in ys[i]
		{
			slice.sort_by(xs[:], proc(a, b: Crossing) -> bool { return (a.x < b.x); });

			j := 0;
			for j < len(xs)
			{
				start_j := j;
				j += 1;

				assert(j < len(xs));
				if (xs[start_j].kind == .L && xs[j].kind == .T) || (xs[start_j].kind == .F && xs[j].kind == .J)
				{
					j += 1;
				}

				for j+1 < len(xs) && ((xs[j].kind == .L && xs[j+1].kind == .J) || (xs[j].kind == .F && xs[j+1].kind == .T))
				{
					j += 2;
				}

				assert(j < len(xs));
				if j+1 < len(xs) && ((xs[j].kind == .L && xs[j+1].kind == .T) || (xs[j].kind == .F && xs[j+1].kind == .J))
				{
					j += 1;
				}

				part1_result += (xs[j].x+1) - xs[start_j].x;
				j += 1;
			}
		}
	}
	fmt.println(len(ys[0]), len(ys[1]));
	fmt.println("Part 2:", part1_result);
}

main :: proc()
{
	input := Input;

	lines := strings.split(input, "\r\n");
	lines = lines[:len(lines)-1];

	Part1(lines);
	Part2(lines);
}


