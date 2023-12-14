#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>

#undef UINT_MAX

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

#define S8_MIN  0x80
#define S16_MIN 0x8000
#define S32_MIN 0x80000000
#define S64_MIN 0x8000000000000000DLL
#define S8_MAX  0x7F
#define S16_MAX 0x7FFF
#define S32_MAX 0x7FFFFFFF
#define S64_MAX 0x7FFFFFFFFFFFFFFFDLL

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#define U8_MAX  0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL

typedef s64 sint;
typedef u64 uint;

#define SINT_MIN S64_MIN
#define SINT_MAX S64_MAX
#define UINT_MIN U64_MIN
#define UINT_MAX U64_MAX

typedef s64 smm;
typedef u64 umm;

typedef u8 bool;
#define true 1
#define false 0

typedef float f32;
typedef double f64;

typedef struct String
{
	u8* data;
	umm size;
} String;

#define STRING(S) (String){ .data = (u8*)(S), .size = sizeof(S)-1 }

#define ASSERT(EX) ((EX) ? 1 : (__debugbreak(), *(volatile int*)0 = 0))
#define NOT_IMPLEMENTED ASSERT(!"NOT_IMPLEMENTED")

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(0[A]))

void*
Align(void* ptr, u8 alignment)
{
	return (void*)(((umm)ptr + (alignment-1)) & ~(alignment-1));
}

bool
ReadInputAligned(int argc, char** argv, String* input, u8 alignment)
{
	bool succeeded = false;

	if (argc != 2) fprintf(stderr, "Invalid number of arguments\n");
	else
	{
		FILE* file;
		if (fopen_s(&file, argv[1], "rb") != 0) fprintf(stderr, "Failed to open input file\n");
		else
		{
			long file_size = 0;
			if (fseek(file, 0, SEEK_END) != 0 || (file_size = ftell(file)) == -1L) fprintf(stderr, "Failed to find input file size\n");
			else
			{
				rewind(file);
				input->size = file_size;
				input->data = calloc(file_size + 2*alignment, 1);
				if (input->data == 0) fprintf(stderr, "Failed to allocate memory for input file\n");
				else
				{
					input->data = Align(input->data, alignment);

					if (fread(input->data, 1, file_size, file) != file_size) fprintf(stderr, "Failed to read input file\n");
					else
					{
						succeeded = true;
					}
				}
			}

			if (fclose(file) != 0)
			{
				succeeded = false;
				fprintf(stderr, "Failed to close input file\n");
			}
		}
	}

	return succeeded;
}

bool
ReadInput(int argc, char** argv, String* input)
{
	return ReadInputAligned(argc, argv, input, 1);
}

sint
AbsS(sint n)
{
  return (n < 0 ? -n : n);
}

sint
MinS(sint a, sint b)
{
  return (a < b ? a : b);
}

sint
MaxS(sint a, sint b)
{
  return (a > b ? a : b);
}

typedef struct V2S
{
  sint x, y;
} V2S;

#define V2S(X, Y) (V2S){ .x = (X), .y = (Y) }

V2S
V2S_Add(V2S v0, V2S v1)
{
  return V2S(v0.x + v1.x, v0.y + v1.y);
}

V2S
V2S_Sub(V2S v0, V2S v1)
{
  return V2S(v0.x - v1.x, v0.y - v1.y);
}

sint
V2S_ManhattanLength(V2S v)
{
  return AbsS(v.x) + AbsS(v.y);
}
