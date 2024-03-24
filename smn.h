#ifndef SMN_HEADER
#define SMN_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

#define S8_MIN  ((s8) 0x80)
#define S16_MIN ((s16)0x8000)
#define S32_MIN ((s32)0x80000000)
#define S64_MIN ((s64)0x8000000000000000DLL)

#define S8_MAX  ((s8) 0x7F)
#define S16_MAX ((s16)0x7FFF)
#define S32_MAX ((s32)0x7FFFFFFF)
#define S64_MAX ((s64)0x7FFFFFFFFFFFFFFFDLL)

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#define U8_MAX  ((u8) 0xFF)
#define U16_MAX ((u16)0xFFFF)
#define U32_MAX ((u32)0xFFFFFFFF)
#define U64_MAX ((u64)0xFFFFFFFFFFFFFFFFULL)

typedef u32 uint;

typedef s64 smm;
typedef u64 umm;

typedef u8 bool;
#define true 1
#define false 0

typedef float f32;
typedef double f64;

#define MAX(A, B) ({                       \
    typeof((A)+(B)) __MAX_A = (A);         \
    typeof((A)+(B)) __MAX_B = (B);         \
    __MAX_A > __MAX_B ? __MAX_A : __MAX_B; \
})

#define MIN(A, B) ({                       \
    typeof((A)+(B)) __MIN_A = (A);         \
    typeof((A)+(B)) __MIN_B = (B);         \
    __MIN_A < __MIN_B ? __MIN_A : __MIN_B; \
})

// TODO: Add safety check
#define ARRAY_SIZE(A) (sizeof(A)/sizeof(0[A]))

#ifndef SMN_NO_ASSERT
#define ASSERT(EX, ...) ((EX) ? 1 : (AssertHandler(__FILE__, __LINE__, #EX, "" __VA_ARGS__), 0))
#else
#define ASSERT(EX, ...)
#endif

void AssertHandler(char* file, uint line, char* expr, char* msg, ...);

typedef struct String
{
	u8* data;
	umm size;
} String;

#define STRING(S) (String){ .data = (u8*)(S), .size = sizeof(S)-1 }

bool Char_IsAlpha(u8 c);
u8 Char_ToLowerUnchecked(u8 c);
u8 Char_ToUpperUnchecked(u8 c);
u8 Char_ToLower(u8 c);
u8 Char_ToUpper(u8 c);

bool String_Match(String s0, String s1);
bool String_MatchCaseInsensitive(String s0, String s1);
String String_EatN(String s, umm amount);
String String_ChopN(String s, umm amount);

bool ReadEntireFileSloppy(char* filename, String* contents);

typedef struct SB__Header
{
  u32 len;
  u32 cap;
} SB__Header;

void** SB__Resize(void** sbuf, umm elem_size, u8 elem_align, umm new_len);

#define SB(T) T*

#define SB__Header(S) ((SB__Header*)(*(S)) - 1)

#define SB_Len(S) ({ (void)**(S); (*(S) == 0 ? 0 : SB__Header(S)->len); })
#define SB_Cap(S) ({ (void)**(S); (*(S) == 0 ? 0 : SB__Header(S)->cap); })

#define SB_Append(S, E) ({                                                 \
    (void)**(S);                                                           \
    SB__Resize((void**)(S), sizeof(**(S)), __alignof(**(S)), SB_Len(S)+1); \
    (*(S))[SB_Len(S)-1] = (E);                                             \
})

#define SB_Prepend(S, E) ({                                                \
    (void)**(S);                                                           \
    SB__Resize((void**)(S), sizeof(**(S)), __alignof(**(S)), SB_Len(S)+1); \
    memmove(*(S)+1, *(S), sizeof(**(S))*(SB_Len(S)-1));                    \
    (*(S))[0] = (E);                                                       \
})

#define SB_UnorderedRemove(S, I) ({                                        \
    (void)**(S);                                                           \
    ASSERT(SB__Header(S)->len > 0);                                        \
    ASSERT((I) >= 0 && (I) < SB_Len(S));                                   \
    (*(S))[I] = (*(S))[SB_Len(S)-1];                                       \
    SB__Header(S)->len -= 1;                                               \
})

#define SB_OrderedRemove(S, I) ({                                          \
    (void)**(S);                                                           \
    ASSERT(SB__Header(S)->len > 0);                                        \
    ASSERT((I) >= 0 && (I) < SB_Len(S));                                   \
    memcpy(*(S)+(I), *(S)+(I)+1, sizeof(**(S))*(SB_Len(S)-((I)+1)));       \
    SB__Header(S)->len -= 1;                                               \
})

#define SB_RemoveLast(S) ({         \
    (void)**(S);                    \
    ASSERT(SB__Header(S)->len > 0); \
    SB__Header(S)->len -= 1;        \
})

#define SB_Free(S) ({ (void)**(S); free((u8*)*(S) - sizeof(SB__Header)); *(S) = 0; })

#ifdef SMN_IMPLEMENTATION

void
AssertHandler(char* file, uint line, char* expr, char* msg, ...)
{
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "%s(%u): Assertion \"%s\" failed\n", file, line, expr);
  if (msg != 0)
  {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
    vfprintf(stderr, msg, args);
#pragma clang diagnostic pop
    putc('\n', stderr);
  }
  va_end(args);

  __debugbreak();
}

bool
Char_IsAlpha(u8 c)
{
  return ((u8)((c&0xDF)-'A') <= (u8)('Z' - 'A'));
}

u8
Char_ToLowerUnchecked(u8 c)
{
  return c & 0xDF;
}

u8
Char_ToUpperUnchecked(u8 c)
{
  return c | 0x20;
}

u8
Char_ToLower(u8 c)
{
  return (Char_IsAlpha(c) ? Char_ToLowerUnchecked(c) : c);
}

u8
Char_ToUpper(u8 c)
{
  return (Char_IsAlpha(c) ? Char_ToUpperUnchecked(c) : c);
}

bool
String_Match(String s0, String s1)
{
	bool result = (s0.size == s1.size);

	for (umm i = 0; i < s0.size && result; ++i)
  {
    result = (s0.data[i] == s1.data[i]);
  }

	return result;
}

bool
String_MatchCaseInsensitive(String s0, String s1)
{
	bool result = (s0.size == s1.size);

	for (umm i = 0; i < s0.size && result; ++i)
  {
    result = (Char_ToLower(s0.data[i]) == Char_ToLower(s1.data[i]));
  }

	return result;
}

String
String_EatN(String s, umm amount)
{
  return (String){
    .data = s.data + amount,
    .size = MAX(s.size, amount) - amount,
  };
}

String
String_ChopN(String s, umm amount)
{
  return (String){
    .data = s.data,
    .size = MAX(s.size, amount) - amount,
  };
}

void**
SB__Resize(void** sbuf, umm elem_size, u8 elem_align, umm new_len)
{
  ASSERT(sizeof(SB__Header) % 8 == 0);
  ASSERT(new_len <= U32_MAX/elem_size);

  if (*sbuf == 0)
  {
    umm new_cap = MAX(new_len, 4);
    ASSERT(new_cap <= U32_MAX);

    *sbuf = (u8*)malloc(sizeof(SB__Header) + new_cap*elem_size) + sizeof(SB__Header);
    SB__Header(sbuf)->cap = (u32)new_cap;
  }
  else if (SB__Header(sbuf)->cap < new_len)
  {
    umm new_cap = 2*SB__Header(sbuf)->cap;
    ASSERT(new_cap <= U32_MAX);

    if (new_cap < new_len) new_cap = new_len;

    *sbuf = (u8*)realloc((u8*)*sbuf - sizeof(SB__Header), sizeof(SB__Header) + new_cap*elem_size) + sizeof(SB__Header);
    SB__Header(sbuf)->cap = (u32)new_cap;
  }

  SB__Header(sbuf)->len = (u32)new_len;

  return sbuf;
}

#endif
#endif
