#include "../aoc.h"

typedef union Hand
{
	struct
	{
		u16 bid;
		u8 cards[5];
	};
	u64 bytes;
} Hand;

void
RevQuicksort(u8* arr, uint size)
{
	if (size < 2) return;

	u8 pivot    = arr[size-1];
	uint cursor = 0;

	for (uint i = 0; i < size; ++i)
	{
		if (arr[i] >= pivot)
		{
			u8 tmp = arr[cursor];
			arr[cursor] = arr[i];
			arr[i]      = tmp;

			++cursor;
		}
	}

	RevQuicksort(arr, cursor - 1);
	RevQuicksort(arr + cursor, size - cursor);
}

bool JokerEnabled = false;

sint
HandScore(Hand* hand)
{
	sint score = 0;

	u8 counts[13] = {0};
	for (uint i = 0; i < 5; ++i) counts[hand->cards[i]] += 1;

	if (JokerEnabled)
	{
		uint joker_count = counts[9];
		counts[9] = 0;

		RevQuicksort(counts, 13);

		if      (counts[0] + joker_count == 5)                   score = 6;
		else if (counts[0] + joker_count == 4)                   score = 5;
		else if (counts[0] + joker_count == 3 && counts[1] == 2) score = 4;
		else if (counts[0] + joker_count == 3)                   score = 3;
		else if (counts[0] == 2 && counts[1] == 2)               score = 2;
		else if (counts[0] + joker_count == 2)                   score = 1;
	}
	else
	{
		RevQuicksort(counts, 13);

		if      (counts[0] == 5)                   score = 6;
		else if (counts[0] == 4)                   score = 5;
		else if (counts[0] == 3 && counts[1] == 2) score = 4;
		else if (counts[0] == 3)                   score = 3;
		else if (counts[0] == 2 && counts[1] == 2) score = 2;
		else if (counts[0] == 2)                   score = 1;
	}

	return score;
}

sint
CompareHands(Hand* h0, Hand* h1)
{
	sint h0_score = HandScore(h0);
	sint h1_score = HandScore(h1);

	sint result = h0_score - h1_score;
	if (result == 0)
	{
		for (uint i = 0; i < 5; ++i)
		{
			if (h0->cards[i] != h1->cards[i])
			{
				sint h0_card = h0->cards[i];
				sint h1_card = h1->cards[i];

				if      (JokerEnabled && h0_card == 9) result = -1;
				else if (JokerEnabled && h1_card == 9) result = 1;
				else                                   result = h0_card - h1_card;

				break;
			}
		}
	}

	return result;
}

void
QuicksortHands(Hand* hands, uint size)
{
	if (size < 2) return;

	Hand* pivot  = &hands[size-1];
	Hand* cursor = hands;

	for (uint i = 0; i < size; ++i)
	{
		if (CompareHands(&hands[i], pivot) <= 0)
		{
			u64 tmp = cursor->bytes;
			cursor->bytes  = hands[i].bytes;
			hands[i].bytes = tmp;

			++cursor;
		}
	}

	uint p = (uint)(cursor - hands) ;
	QuicksortHands(hands, p - 1);
	QuicksortHands(cursor, size - p);
}

void
InsertionSortHands(Hand* hands, uint size)
{
	for (uint i = 0; i < size; ++i)
	{
		uint j = i;
		while (j > 0 && CompareHands(&hands[j-1], &hands[j]) > 0)
		{
			u64 tmp = hands[j-1].bytes;
			hands[j-1].bytes = hands[j].bytes;
			hands[j].bytes   = tmp;

			--j;
		}
	}
}

void
PrintHand(Hand* hand)
{
	for (uint i = 0; i < 5; ++i)
	{
		u8 c;
		switch (hand->cards[i])
		{
			case 12: c = 'A'; break;
			case 11: c = 'K'; break;
			case 10: c = 'Q'; break;
			case  9: c = 'J';  break;
			case  8: c = 'T';  break;
			default: c = '0' + hand->cards[i]+2; break;
		}

		printf("%c", c);
	}
}

int
main(int argc, char** argv)
{
	String input;
	if (!ReadInput(argc, argv, &input)) return 0;

	Hand* hands = malloc(sizeof(Hand)*1000);
	uint hand_count = 0;

	for (uint i = 0; i < input.size;)
	{
		for (uint j = 0; input.data[i] != ' '; ++i, ++j)
		{
			switch (input.data[i])
			{
				case 'A': hands[hand_count].cards[j] = 12; break;
				case 'K': hands[hand_count].cards[j] = 11; break;
				case 'Q': hands[hand_count].cards[j] = 10; break;
				case 'J': hands[hand_count].cards[j] = 9;  break;
				case 'T': hands[hand_count].cards[j] = 8;  break;
				default:  hands[hand_count].cards[j] = (input.data[i]&0xF)-2; break;
			}
		}

		uint num = 0;
		for (; i < input.size && input.data[i] != '\r'; ++i) num = num*10 + (input.data[i]&0xF);

		ASSERT(num <= U16_MAX);
		hands[hand_count].bid = (u16)num;

		++hand_count;
		i += 2;
	}

	JokerEnabled = true;
	for (uint i = 0; i < hand_count; ++i)
	{
		sint r = HandScore(hands + i);
	}
	JokerEnabled = false;

	InsertionSortHands(hands, hand_count);

	uint part1_result = 0;
	for (uint i = 0; i < hand_count; ++i) part1_result += hands[i].bid*(i+1);

	JokerEnabled = true;
	InsertionSortHands(hands, hand_count);

	uint part2_result = 0;
	for (uint i = 0; i < hand_count; ++i) part2_result += hands[i].bid*(i+1);
	

	printf("Part 1: %llu\n", part1_result);
	printf("Part 2: %llu\n", part2_result);

	return 0;
}
