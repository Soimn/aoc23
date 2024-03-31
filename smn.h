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

#define S8_MIN  (0x80)
#define S16_MIN (0x8000)
#define S32_MIN (0x80000000)
#define S64_MIN (0x8000000000000000DLL)
#define S128_MIN ((s128)((~(u128)0 >> 1) + 1)

#define S8_MAX  (0x7F)
#define S16_MAX (0x7FFF)
#define S32_MAX (0x7FFFFFFF)
#define S64_MAX (0x7FFFFFFFFFFFFFFFDLL)
#define S128_MAX ((s128)(~(u128)0 >> 1))

typedef unsigned __int8   u8;
typedef unsigned __int16  u16;
typedef unsigned __int32  u32;
typedef unsigned __int64  u64;
typedef unsigned __int128 u128;

#define U8_MAX  (0xFF)
#define U16_MAX (0xFFFF)
#define U32_MAX (0xFFFFFFFF)
#define U64_MAX (0xFFFFFFFFFFFFFFFFULL)
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
    typeof((A)+(B)) MAX___A = (A);         \
    typeof((A)+(B)) MAX___B = (B);         \
    MAX___A > MAX___B ? MAX___A : MAX___B; \
})

#define MIN(A, B) ({                       \
    typeof((A)+(B)) MIN___A = (A);         \
    typeof((A)+(B)) MIN___B = (B);         \
    MIN___A < MIN___B ? MIN___A : MIN___B; \
})

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(0[A]))

#define CONCAT__(A, B) A##B
#define CONCAT_(A, B) CONCAT__(A, B)
#define CONCAT(A, B) CONCAT_(A, B)

#define ALIGN_FORWARD(N, A) (((N) + ((A)-1)) & ~(typeof((N)+(A)))((A)-1))
#define ALIGN_BACKWARD(N, A) ((N) & ~(typeof((N)+(A)))((A)-1))

#define IS_POW_2(N) (((N) & ((N)-1)) == 0 && (N) != 0)

#ifndef SMN_NO_ASSERT
#define ASSERT(EX, ...) ((EX) ? 1 : (AssertHandler(__FILE__, __LINE__, (char*)__FUNCTION__, #EX, "" __VA_ARGS__), 0))
#else
#define ASSERT(EX, ...)
#endif

void AssertHandler(char* file, uint line, char* func, char* expr, char* msg, ...);

#define NOT_IMPLEMENTED ASSERT(false, "NOT_IMPLEMENTED")

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
SMN_INLINE u64    String_FNV                 (String s);

// -------------------------------------------------
// SB - Stretchy Buffer
// u32 SB_Len(SB(T)* sbuf)
// u32 SB_Cap(SB(T)* sbuf)
// void SB_Append(SB(T)* sbuf, T elem)
// void SB_Prepend(SB(T)* sbuf, T elem)
// void SB_UnorderedRemove(SB(T)* sbuf, u32 index)
// void SB_OrderedRemove(SB(T)* sbuf, u32 index)
// void SB_RemoveLast(SB(T)* sbuf)
// void SB_Free(SB(T)* sbuf)

typedef struct SB__Header
{
  u32 len;
  u32 cap;
} SB__Header;

void** SB__Reserve(void** sbuf, umm elem_size, u8 elem_align, umm new_len);

#define SB(T) T*

#define SB__Header(S) ((SB__Header*)(*(S)) - 1)

#define SB__EnsureDoublePointer(S) ({ if(*(S)) (void)**(S); })

#define SB_Len(S) ({ SB__EnsureDoublePointer(S); (*(S) == 0 ? 0 : SB__Header(S)->len); })
#define SB_Cap(S) ({ SB__EnsureDoublePointer(S); (*(S) == 0 ? 0 : SB__Header(S)->cap); })

#define SB_Append(S, E) ({                                                 \
    SB__EnsureDoublePointer(S);                                            \
    SB__Reserve((void**)(S), sizeof(**(S)), __alignof(**(S)), SB_Len(S));  \
    (*(S))[SB__Header(S)->len++] = (E);                                    \
    (void)0;                                                               \
})

#define SB_Prepend(S, E) ({                                                          \
    SB__EnsureDoublePointer(S);                                                      \
    SB__Reserve((void**)(S), sizeof(**(S)), __alignof(**(S)), ++SB__Header(S)->len); \
    memmove(*(S)+1, *(S), sizeof(**(S))*(SB_Len(S)-1));                              \
    (*(S))[0] = (E);                                                                 \
    (void)0;                                                                         \
})

#define SB_UnorderedRemove(S, I) ({                                        \
    SB__EnsureDoublePointer(S);                                            \
    ASSERT(SB__Header(S)->len > 0);                                        \
    ASSERT((I) >= 0 && (I) < SB_Len(S));                                   \
    (*(S))[I] = (*(S))[SB_Len(S)-1];                                       \
    SB__Header(S)->len -= 1;                                               \
    (void)0;                                                               \
})

#define SB_OrderedRemove(S, I) ({                                          \
    SB__EnsureDoublePointer(S);                                            \
    ASSERT(SB__Header(S)->len > 0);                                        \
    ASSERT((I) >= 0 && (I) < SB_Len(S));                                   \
    memcpy(*(S)+(I), *(S)+(I)+1, sizeof(**(S))*(SB_Len(S)-((I)+1)));       \
    SB__Header(S)->len -= 1;                                               \
    (void)0;                                                               \
})

#define SB_RemoveLast(S) ({         \
    SB__EnsureDoublePointer(S);     \
    ASSERT(SB__Header(S)->len > 0); \
    SB__Header(S)->len -= 1;        \
    (void)0;                        \
})

#define SB_Free(S) ({                       \
    SB__EnsureDoublePointer(S);             \
    if (*(S) != 0)                          \
    {                                       \
      free((u8*)*(S) - sizeof(SB__Header)); \
    }                                       \
    *(S) = 0;                               \
    (void)0;                                \
})

// -------------------------------------------------
// SBMap - Stretchy buffer based hashmap
// void SBMap_Create(SBMap(K, V)* map, u64 (*hash_func)(void*), bool (*match_func)(void* a, void* b),
//                   u8 key_size, u8 value_offset, u16 value_size)
// void SBMap_CreateT(SBMap(K, V)* map, T)
// void SBMap_Destroy(SBMap(K, V)* map)
// bool SBMap_Get(SBMap(K, V)* map, K key, V* value)
// void SBMap_Put(SBMap(K, V)* map, K key, V value)
// bool SBMap_Has(SBMap(K, V)* map, K key)
// void SBMap_Del(SBMap(K, V)* map, K key)
// void SBMap_Clr(SBMap(K, V)* map)

typedef struct SBMap__Header
{
  u64 (*hash_func)(void* obj);
  bool (*match_func)(void* a, void* b);
  u32 len;
  u32 cap;
  u8 key_size;
  u8 value_offset;
  u16 value_size;
  u32 entry_size;
} SBMap__Header;

void SBMap__Create(void** map, u64 (*hash_func)(void*), bool (*match_func)(void* a, void* b),
                   u8 key_size, u8 value_offset, u16 value_size);
bool SBMap__Get(void** map, void* key, void* value);
void SBMap__Put(void** map, void* key, void* value);
bool SBMap__Has(void** map, void* key);
void SBMap__Del(void** map, void* key);
void SBMap__Clr(void** map);

#define SBMap__HashMatchImpl(T, HE, ME)                                                    \
  u64 SBMap__Hash##T(void* _key) { T key = *(T*)_key; return (u64)(HE); }                 \
  bool SBMap__Match##T(void* _a, void* _b) { T a = *(T*)_a; T b = *(T*)_b; return (ME); } \

SBMap__HashMatchImpl(int,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(uint, String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(s8,   String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(s16,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(s32,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(s64,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(u8,   String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(u16,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(u32,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(u64,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(smm,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(umm,  String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), a == b)
SBMap__HashMatchImpl(bool, String_FNV((String){ .data = (u8*)&key, .size = sizeof(key) }), !!a == !!b)
SBMap__HashMatchImpl(String, String_FNV(key), String_Match(a, b))

#define SBMap(K, V) struct { u64 hash; K key; V value; }*

#define SBMap__MaxLoadPercent 69

#define SBMap__Header(M) ((SBMap__Header*)*(M)-1)
#define SBMap__KeyType(M) typeof((*(M))->key)
#define SBMap__ValueType(M) typeof((*(M))->value)

#define SBMap__EmptyHash   (1ULL << 63)
#define SBMap__IsOccupied(S) (!(*(S) >> 63))

#define SBMap_Create(M, H, C) ({                                                            \
    (void)**(M);                                                                            \
    ASSERT(__alignof(typeof((*(M))->key)) <= 8 && __alignof(typeof((*(M))->key)) <= 8,      \
        "Key and value must have alignments less than or equal to 8");                      \
    ASSERT(sizeof((*(M))->key) < U8_MAX, "Size of key must be less than 256 bytes");        \
    ASSERT(sizeof((*(M))->value) < U16_MAX, "Size of value must be less than 65536 bytes"); \
    SBMap__Create((void**)(M), (H), (C), (u8)sizeof((*(M))->key),                           \
        (u8)offsetof(typeof(**(M)), value), (u16)sizeof((*(M))->value));                    \
})

#define SBMap_CreateT(M, T) SBMap_Create(M, CONCAT(SBMap__Hash, T), CONCAT(SBMap__Match, T))

// TODO:
#define SBMap_Destroy(M) ({ (void)**(M); })

#define SBMap_Get(M, K, V) ({                                 \
    (void)**(M);                                              \
    typeof(SBMap__KeyType(M)) SBMap__Get_key    = (K);        \
    typeof(SBMap__ValueType(M)*) SBMap__Get_val = (V);        \
    SBMap__Get((void**)(M), &SBMap__Get_key, SBMap__Get_val); \
})

#define SBMap_Put(M, K, V) ({                                    \
    (void)**(M);                                                 \
    typeof(SBMap__KeyType(M)) SBMap__Put_key   = (K);            \
    typeof(SBMap__ValueType(M)) SBMap__Put_val = (V);            \
    SBMap__Put((void**)(M), &SBMap__Put_key, &SBMap__Put_val);   \
    (void)0;                                                     \
})

#define SBMap_Has(M, K) ({                          \
    (void)**(M);                                    \
    typeof(SBMap__KeyType(M)) SBMap__Has_key = (K); \
    SBMap__Has((void**)(M), &SBMap__Has_key);       \
})

#define SBMap_Del(M, K) ({                          \
    (void)**(M);                                    \
    typeof(SBMap__KeyType(M)) SBMap__Del_key = (K); \
    SBMap__Del((void**)(M), &SBMap__Del_key);       \
    (void)0;                                        \
})

#define SBMap_Clr(M) ({      \
    (void)**(M);             \
    SBMap__Clr((void**)(M)); \
})

#ifdef SMN_IMPLEMENTATION

// TODO: failed asserts on paramteres should report the caller's file and line number, not the callee's
void __attribute__((format(printf, 5, 6)))
AssertHandler(char* file, uint line, char* func, char* expr, char* msg, ...)
{
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "%s(%u), %s: Assertion \"%s\" failed\n", file, line, func, expr);
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
  return c | 0x20;
}

SMN_INLINE u8
Char_ToUpperUnchecked(u8 c)
{
  return c & 0xDF;
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
    .data = s.data + (s.size - amount),
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

SMN_INLINE u64
String_FNV(String s)
{
  // NOTE: FNV-1 from explanation https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function 
  u64 hash = 0XCBF29CE484222325ULL;
  for (umm i = 0; i < s.size; ++i)
  {
    hash *= 0x00000100000001B3ULL;
    hash ^= s.data[i];
  }

  return hash;
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
SB__Reserve(void** sbuf, umm elem_size, u8 elem_align, umm min_cap)
{
  ASSERT(sizeof(SB__Header) % 8 == 0);
  ASSERT(min_cap <= U32_MAX/elem_size);

  if (*sbuf == 0)
  {
    umm new_cap = MAX(min_cap, 4);
    ASSERT(new_cap <= U32_MAX);

    *sbuf = (u8*)malloc(sizeof(SB__Header) + new_cap*elem_size) + sizeof(SB__Header);
    SB__Header(sbuf)->len = 0;
    SB__Header(sbuf)->cap = (u32)new_cap;
  }
  else if (SB__Header(sbuf)->cap < min_cap)
  {
    umm new_cap = 2*SB__Header(sbuf)->cap;
    ASSERT(new_cap <= U32_MAX);

    if (new_cap < min_cap) new_cap = min_cap;

    *sbuf = (u8*)realloc((u8*)*sbuf - sizeof(SB__Header), sizeof(SB__Header) + new_cap*elem_size) + sizeof(SB__Header);
    SB__Header(sbuf)->cap = (u32)new_cap;
  }

  return sbuf;
}

inline u64
SBMap__HashKey(void** map, void* key)
{
  return SBMap__Header(map)->hash_func(key) >> 1;
}

void
SBMap__Create(void** map, u64 (*hash_func)(void*), bool (*match_func)(void* a, void* b),
              u8 key_size, u8 value_offset, u16 value_size)
{
  u32 init_cap = 128;
  u32 entry_size = ALIGN_FORWARD(value_offset + value_size, 8);

  SBMap__Header* header = malloc(sizeof(SBMap__Header) + init_cap*entry_size);
  *header = (SBMap__Header){
    .hash_func    = hash_func,
    .match_func   = match_func,
    .len          = 0,
    .cap          = init_cap,
    .key_size     = key_size,
    .value_offset = value_offset,
    .value_size   = value_size,
    .entry_size   = entry_size,
  };

  void* scan = (u8*)(header+1);
  for (u32 i = 0; i < init_cap; ++i)
  {
    *(u64*)(scan + i*entry_size) = SBMap__EmptyHash;
  }

  *map = (void*)(header+1);
}

void
SBMap__Resize(void** map, u32 new_cap)
{
  ASSERT(new_cap > SBMap__Header(map)->len);

  SBMap__Header* header = malloc(sizeof(SBMap__Header) + new_cap*SBMap__Header(map)->entry_size);
  *header = *SBMap__Header(map);
  header->len = 0;
  header->cap = new_cap;

  void* new_map = (void*)(header+1);

  for (uint i = 0; i < SBMap__Header(map)->len; ++i)
  {
    u64* slot = (u64*)((void*)*map + i*SBMap__Header(map)->entry_size);

    void* key   = slot+1;
    void* value = (u8*)slot + SBMap__Header(map)->value_offset;

    SBMap__Put(&new_map, key, value);
  }

  free(*map);
  *map = new_map;
}

inline u64*
SBMap__Walk(void** map, u64 hash, void* key)
{
  ASSERT(IS_POW_2(SBMap__Header(map)->cap));

  for (uint scan = hash & (SBMap__Header(map)->cap-1);; scan = (scan+1) & (SBMap__Header(map)->cap-1))
  {
    u64* slot = (u64*)((void*)*map + scan*SBMap__Header(map)->entry_size);

    if (*slot == SBMap__EmptyHash || (*slot == hash && SBMap__Header(map)->match_func(slot+1, key)))
    {
      return slot;
    }
  }
}
bool
SBMap__Get(void** map, void* key, void* value)
{
  u64 hash  = SBMap__HashKey(map, key);
  u64* slot = SBMap__Walk(map, hash, key);

  bool is_occ = SBMap__IsOccupied(slot);
  if (is_occ)
  {
    memcpy(value, (u8*)slot + SBMap__Header(map)->value_offset, SBMap__Header(map)->value_size);
  }

  return is_occ;
}

void
SBMap__Put(void** map, void* key, void* value)
{
  if (SBMap__Header(map)->len*100 >= SBMap__Header(map)->cap*SBMap__MaxLoadPercent)
  {
    SBMap__Resize(map, SBMap__Header(map)->cap*2);
  }

  u64 hash  = SBMap__HashKey(map, key);
  u64* slot = SBMap__Walk(map, hash, key);
  
  *slot = hash;
  memcpy(slot+1, key, SBMap__Header(map)->key_size);
  memcpy((u8*)slot + SBMap__Header(map)->value_offset, value, SBMap__Header(map)->value_size);

  SBMap__Header(map)->len += 1;
}

bool
SBMap__Has(void** map, void* key)
{
  u64 hash  = SBMap__HashKey(map, key);
  u64* slot = SBMap__Walk(map, hash, key);

  return SBMap__IsOccupied(slot);
}

void
SBMap__Del(void** map, void* key)
{
  // NOTE: Adapted from https://en.wikipedia.org/wiki/Open_addressing

  u64 hash  = SBMap__HashKey(map, key);
  u64* slot = SBMap__Walk(map, hash, key);

  *slot = SBMap__EmptyHash;
  SBMap__Header(map)->len -= 1;

  uint entry_size = SBMap__Header(map)->entry_size;

  uint probe_idx = (uint)((void*)slot - *map)/entry_size + 1;

  for (uint i = probe_idx, j = probe_idx;; i = (i+1) & ~SBMap__Header(map)->cap)
  {
    u64* i_slot = *map + i*entry_size;

    if (*i_slot == SBMap__EmptyHash) break;
    else
    {
      u64 h = *i_slot & (SBMap__Header(map)->cap-1);

      if ((j < i && h > j && h <= i) || (i < j && (h > j || h <= i))) continue;
      else
      {
        memcpy(*map + j*entry_size, i_slot, entry_size);
        *i_slot = SBMap__EmptyHash;
        j = i;
      }
    }
  }
}

void
SBMap__Clr(void** map)
{
  for (uint i = 0; i < SBMap__Header(map)->cap; ++i)
  {
    *(u64*)((void*)*map + i*SBMap__Header(map)->entry_size) = SBMap__EmptyHash;
  }

  SBMap__Header(map)->len = 0;
}

#endif
#endif
