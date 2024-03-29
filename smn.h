#ifndef SMN_HEADER
#define SMN_HEADER

#if defined(SMN_INLINE_IMPLEMENTATION) && !defined(SMN_IMPLEMENTATION)
#define SMN_IMPLEMENTATION
#endif

#ifdef SMN_INLINE_IMPLEMENTATION
#define SMN_INLINE inline
#else
#define SMN_INLINE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef signed __int8   s8;
typedef signed __int16  s16;
typedef signed __int32  s32;
typedef signed __int64  s64;
typedef signed __int128 s128;

#define S8_MIN   ((s8)  ((~(u8)  0 >> 1) + 1)
#define S16_MIN  ((s16) ((~(u16) 0 >> 1) + 1)
#define S32_MIN  ((s32) ((~(u32) 0 >> 1) + 1)
#define S64_MIN  ((s64) ((~(u64) 0 >> 1) + 1)
#define S128_MIN ((s128)((~(u128)0 >> 1) + 1)

#define S8_MAX   ((s8)  (~(u8)  0 >> 1))
#define S16_MAX  ((s16) (~(u16) 0 >> 1))
#define S32_MAX  ((s32) (~(u32) 0 >> 1))
#define S64_MAX  ((s64) (~(u64) 0 >> 1))
#define S128_MAX ((s128)(~(u128)0 >> 1))

typedef unsigned __int8   u8;
typedef unsigned __int16  u16;
typedef unsigned __int32  u32;
typedef unsigned __int64  u64;
typedef unsigned __int128 u128;

#define U8_MAX   (~(u8)0)
#define U16_MAX  (~(u16)0)
#define U32_MAX  (~(u32)0)
#define U64_MAX  (~(u64)0)
#define U128_MAX (~(u128)0)

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

SMN_INLINE bool Char_IsAlpha         (u8 c);
SMN_INLINE u8   Char_ToLowerUnchecked(u8 c);
SMN_INLINE u8   Char_ToUpperUnchecked(u8 c);
SMN_INLINE u8   Char_ToLower         (u8 c);
SMN_INLINE u8   Char_ToUpper         (u8 c);
SMN_INLINE bool Char_IsWhitespace    (u8 c);
SMN_INLINE bool Char_IsDigit         (u8 c);
SMN_INLINE bool Char_IsHexAlphaDigit (u8 c);

SMN_INLINE bool   String_Match               (String s0, String s1);
SMN_INLINE bool   String_MatchCaseInsensitive(String s0, String s1);
SMN_INLINE String String_EatN                (String s, umm amount);
SMN_INLINE String String_ChopN               (String s, umm amount);
SMN_INLINE String String_EatMaxN             (String s, umm amount);
SMN_INLINE String String_ChopMaxN            (String s, umm amount);
SMN_INLINE String String_FirstN              (String s, umm amount);
SMN_INLINE String String_LastN               (String s, umm amount);
SMN_INLINE String String_FirstMaxN           (String s, umm amount);
SMN_INLINE String String_LastMaxN            (String s, umm amount);
SMN_INLINE String String_EatWhitespace       (String s);
SMN_INLINE smm    String_FindFirstChar       (String s, u8 c);
SMN_INLINE smm    String_FindLastChar        (String s, u8 c);
SMN_INLINE umm    String_CopyToBuffer        (String s, u8* buffer, umm buffer_size);

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

SMN_INLINE bool
Char_IsAlpha(u8 c)
{
  return ((u8)((c&0xDF)-'A') <= (u8)('Z' - 'A'));
}

SMN_INLINE u8
Char_ToLowerUnchecked(u8 c)
{
  return c & 0xDF;
}

SMN_INLINE u8
Char_ToUpperUnchecked(u8 c)
{
  return c | 0x20;
}

SMN_INLINE u8
Char_ToLower(u8 c)
{
  return (Char_IsAlpha(c) ? Char_ToLowerUnchecked(c) : c);
}

SMN_INLINE u8
Char_ToUpper(u8 c)
{
  return (Char_IsAlpha(c) ? Char_ToUpperUnchecked(c) : c);
}

SMN_INLINE bool
Char_IsWhitespace(u8 c)
{
  return (c == ' ' || (u8)(c - '\t') <= (u8)('\r'-'\t'));
}

SMN_INLINE bool
Char_IsDigit(u8 c)
{
  return ((u8)(c-'0') < (u8)10);
}

SMN_INLINE bool
Char_IsHexAlphaDigit(u8 c)
{
  return ((u8)((c&0xDF)-'A') <= (u8)('F' - 'A'));
}

SMN_INLINE bool
String_Match(String s0, String s1)
{
  bool result = (s0.size == s1.size);

  for (umm i = 0; i < s0.size && result; ++i)
  {
    result = (s0.data[i] == s1.data[i]);
  }

  return result;
}

SMN_INLINE bool
String_MatchCaseInsensitive(String s0, String s1)
{
  bool result = (s0.size == s1.size);

  for (umm i = 0; i < s0.size && result; ++i)
  {
    result = (Char_ToLower(s0.data[i]) == Char_ToLower(s1.data[i]));
  }

  return result;
}

SMN_INLINE String
String_EatN(String s, umm amount)
{
  ASSERT(s.size >= amount);
  return (String){
    .data = s.data + amount,
    .size = s.size - amount,
  };
}

SMN_INLINE String
String_ChopN(String s, umm amount)
{
  ASSERT(s.size >= amount);
  return (String){
    .data = s.data,
    .size = s.size - amount,
  };
}

SMN_INLINE String
String_EatMaxN(String s, umm amount)
{
  return (String){
    .data = s.data + amount,
    .size = MAX(s.size, amount) - amount,
  };
}

SMN_INLINE String
String_ChopMaxN(String s, umm amount)
{
  return (String){
    .data = s.data,
    .size = MAX(s.size, amount) - amount,
  };
}

SMN_INLINE String
String_FirstN(String s, umm amount)
{
  ASSERT(amount < s.size);
  return (String){
    .data = s.data,
    .size = amount,
  };
}

SMN_INLINE String
String_LastN(String s, umm amount)
{
  ASSERT(amount < s.size);
  return (String){
    .data = s.data + (amount - s.size),
    .size = amount,
  };
}

SMN_INLINE String
String_FirstMaxN(String s, umm amount)
{
  return (String){
    .data = s.data,
    .size = MIN(s.size, amount),
  };
}

SMN_INLINE String
String_LastMaxN(String s, umm amount)
{
  amount = MIN(s.size, amount);

  return (String){
    .data = s.data + (s.size - amount),
    .size = amount,
  };
}

SMN_INLINE String
String_EatWhitespace(String s)
{
  umm i = 0;
  while (i < s.size && Char_IsWhitespace(s.data[i])) ++i;

  return String_EatN(s, i);
}

SMN_INLINE smm
String_FindFirstChar(String s, u8 c)
{
  smm found_idx = -1;

  for (umm i = 0; i < s.size; ++i)
  {
    if (s.data[i] == c)
    {
      found_idx = (smm)i;
      break;
    }
  }

  return found_idx;
}

SMN_INLINE smm
String_FindLastChar(String s, u8 c)
{
  smm found_idx = -1;

  for (umm i = s.size-1; i < s.size; --i)
  {
    if (s.data[i] == c)
    {
      found_idx = (smm)i;
      break;
    }
  }

  return found_idx;
}

SMN_INLINE umm
String_CopyToBuffer(String s, u8* buffer, umm buffer_size)
{
  umm to_copy = MIN(s.size, buffer_size);

  for (umm i = 0; i < to_copy; ++i) buffer[i] = s.data[i];

  return to_copy;
}

SMN_INLINE String
String_EatU64(String s, uint base, u64* out, bool* did_overflow)
{
  ASSERT(base >= 2 && base <= 10 || base == 16);

  u64 result  = 0;
  u8 overflow = 0;
  umm i       = 0;

  if (base == 16)
  {
    for (; i < s.size; ++i)
    {
      u8 c = s.data[i];
      umm digit;

      if      (Char_IsDigit(c))         digit = c&0xF;
      else if (Char_IsHexAlphaDigit(c)) digit = (c&0x1F) + 9;
      else                              break;

      overflow |= result >> 60;
      result    = (result << 4) | digit;
    }
  }
  else
  {
    for (; i < s.size; ++i)
    {
      u8 c = s.data[i];
      umm digit;

      if (Char_IsDigit(c) && c < '0'+base) digit = c&0xF;
      else                                 break;
      
      overflow |= (result > U64_MAX/base);
      result *= base;
      overflow |= (U64_MAX - result < digit);
      result += digit;
    }
  }

  *out = result;

  if (did_overflow != 0) *did_overflow = (overflow != 0);

  return String_EatN(s, i);
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
