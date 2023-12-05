#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef s64 sint;
typedef u64 uint;

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
