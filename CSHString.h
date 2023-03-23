#ifndef CSH_STRING_H
#define CSH_STRING_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

typedef char CSHChar_t;
typedef char* CSHCharPtr_t;
typedef const char* CSHConstCharPtr_t;

// [ extern const size_t CSH_STRING_MAX_STACK_CHAR_COUNT ]
// The maximum number of characters a string can have stored on the stack (2KB).

extern const size_t CSH_STRING_NPOS; // = ~(0);
extern const size_t CSH_CHAR_SIZE; // = sizeof(CSHChar_t);
extern const size_t CSH_STRING_MAX_STACK_CHAR_COUNT; // = ((2048 / CSH_CHAR_SIZE) - 1);

// [ #define CSH_STRING_MAX_CSTR_CHAR_COUNT_M ]
// The general maximum size in characters a cstr should be, not including the null terminating character.

// TODO: Add support for alloca to all CSH function which can support it.
#define CSH_STRING_ALLOCA_ENABLED_M 1
#define CSH_STRING_ASSERT_ENABLED_M 1
#define CSH_STRING_MAX_CSTR_CHAR_COUNT_M 2047
#define CSH_STRING_DEFAULT_M (S_CSHString){NULL, 0, 0, 0, 0, (CSH_STRING_MAX_CSTR_CHAR_COUNT_M + 1)}
#define CSH_STRING_ERROR_M(in_errorCode) (S_CSHString){NULL, in_errorCode, 0, 0, 0, (CSH_STRING_MAX_CSTR_CHAR_COUNT_M + 1)}

// Need a generalised alloca function, as its definition can change between OS's.
void* CSH_alloca(size_t in_size);

// Uses either calloc or alloca, depending on the value passed to in_status and whether CSH_ALLOCA_ENABLED = 0 or 1.
// in_allocaDefault decides whether m_status needs to be explicitly set to CSHSSC_USE_ALLOCA to use alloca.
// in_allocaDefault = true, means that it does not need to be set to CSHSSC_USE_ALLOCA, false means it does.
// in_status is passed by reference, with it being set to CSHSSC_USE_ALLOCA if CSH_alloca was used or CSHSSC_NONE if calloc was used.
// This is a macro function, to ensure CSH_alloca is called on the correct stack frame. 
#define CSH_STRING_CALLOC_MF(in_ptr, in_num, in_size, in_status, in_allocaDefault) \
{ \
    if (in_ptr == NULL) \
    { \
        if ((in_num <= (CSH_STRING_MAX_STACK_CHAR_COUNT + 1)) && CSH_STRING_ALLOCA_ENABLED_M && \
        ((in_allocaDefault && *in_status != CSHSSC_DONT_USE_ALLOCA) || (!in_allocaDefault && *in_status == CSHSSC_USE_ALLOCA))) \
        { \
            size_t sizeInBytes = in_num * in_size; \
            in_ptr = (CSHCharPtr_t)CSH_alloca(sizeInBytes); \
            \
            if (in_ptr != NULL) \
            { \
                *in_status = CSHSSC_USE_ALLOCA; \
                for (size_t i = 0; i < sizeInBytes; i++) \
                { \
                    *(uint8_t*)(in_ptr + i) = 0; \
                } \
            } \
        } \
        else \
        { \
            if (*in_status == CSHSSC_USE_ALLOCA) \
            { \
                *in_status = CSHSSC_NONE; \
            } \
            in_ptr = (CSHCharPtr_t)calloc(in_num, in_size); \
        } \
    } \
}

enum E_CSHStringStatusCodes
{
    CSHSSC_CSTR_DOESNT_FIT = -3,
    CSHSSC_BAD_INPUT_ARG,
    CSHSSC_BAD_INPUT_STR,
    CSHSSC_NONE,
    CSHSSC_ALREADY_RESERVED,
    CSHSSC_USE_ALLOCA,
    CSHSSC_DONT_USE_ALLOCA
};

// [ typedef struct S_CSHString ]
// m_strPtr: Pointer to the string in memory.
// m_status: The current status of the string.
// m_size: The size of the string not including the null terminator.
// m_nullSize: The size of the string including the null terminator.
// m_capacity: The amount of characters the string can store in its current allocated memory.
// m_maxCstrSize: 
//  The maximum size a cstr can be, when a CSH string function is called that uses one.
//  1 is added to this, to account for the null terminating character needed for strnlen.
typedef struct 
{
    CSHCharPtr_t m_strPtr;
    int8_t m_status; 
    size_t m_size;
    size_t m_nullSize;
    size_t m_capacity;
    size_t m_maxCstrSize;
} S_CSHString;

// [ S_CSHString CSH_string_create_cstr(CSHConstCharPtr_t in_str, size_t in_maxSize) ]
// in_maxSize, is the maximum number of characters not including the null terminating character.

// [ int8_t CSH_string_clear(S_CSHString* in_this, bool in_freeMemory) ]
// Setting in_freeMemory to true, will both clear the string and the memory it occupies.

// [ int8_t CSH_string_set_max_cstr_size(S_CSHString* in_this, size_t in_maxSize) ]
// Sets the maximum size in characters, not including the null terminating character, a cstr can be in an operation which contains one.

S_CSHString CSH_string_create(S_CSHString* in_str);
S_CSHString CSH_string_create_cstr(CSHConstCharPtr_t in_str, size_t in_maxSize);
int8_t CSH_string_free(S_CSHString* in_this); 
int8_t CSH_string_clear(S_CSHString* in_this, bool in_freeMemory);
int8_t CSH_string_set_max_cstr_size(S_CSHString* in_this, size_t in_maxSize);

// [ int8_t CSH_string_cstr_fit(S_CSHString* in_this, CSHConstCharPtr_t in_str) ]
// Determines whether a cstr's size is within the maximum limit dictated by m_maxCstrSize.

// [ size_t CSH_cstr_size(CSHConstCharPtr_t in_str, size_t in_maxSize) ]
// in_maxSize determines the maximum number of characters (not including the null terminator) which will be queried. 

// [ size_t CSH_string_size_bytes(S_CSHString* in_this) ]
// Does not include the null terminating character.

// [ size_t CSH_string_null_size_bytes(S_CSHString* in_this) ]
// Does include the null terminating character.

int8_t CSH_string_cstr_fit(S_CSHString* in_this, CSHConstCharPtr_t in_str);
size_t CSH_cstr_size(CSHConstCharPtr_t in_str, size_t in_maxSize);
size_t CSH_string_size_bytes(S_CSHString* in_this);
size_t CSH_string_null_size_bytes(S_CSHString* in_this);

int8_t CSH_string_assign(S_CSHString* in_this, S_CSHString* in_str);
int8_t CSH_string_assign_cstr(S_CSHString* in_this, CSHConstCharPtr_t in_str);

// [ S_CSHString CSH_string_create_concat_cstr(CSHConstCharPtr_t in_strOne, CSHConstCharPtr_t in_strTwo, size_t in_maxSizeOne, size_t in_maxSizeTwo) ]
// in_maxSize = the maximum size of the inputted cstr in characters, not including the null terminator. 

S_CSHString CSH_string_create_concat(S_CSHString* in_strOne, S_CSHString* in_strTwo);
S_CSHString CSH_string_create_concat_cstr(CSHConstCharPtr_t in_strOne, CSHConstCharPtr_t in_strTwo, size_t in_maxSizeOne, size_t in_maxSizeTwo);
S_CSHString CSH_string_create_concat_left_cstr(CSHConstCharPtr_t in_strOne, S_CSHString* in_strTwo);
S_CSHString CSH_string_create_concat_right_cstr(S_CSHString* in_strOne, CSHConstCharPtr_t in_strTwo);

int8_t CSH_string_concat_left(S_CSHString* in_this, S_CSHString* in_str);
int8_t CSH_string_concat_right(S_CSHString* in_str, S_CSHString* in_this);
int8_t CSH_string_concat_left_cstr(CSHConstCharPtr_t in_str, S_CSHString* in_this);
int8_t CSH_string_concat_right_cstr(S_CSHString* in_this, CSHConstCharPtr_t in_str);

// [ int8_t CSH_string_reserve(S_CSHString* in_this, size_t in_size) ]
// in_size = number of characters to reserve memory for, not including the null terminator.

// [ int8_t CSH_string_shrink_to_fit(S_CSHString* in_this) ]
// Reduces the capacity of the string (memory), so it just fits the strings contents.

// [ int8_t CSH_string_resize(S_CSHString* in_this, size_t in_size, CSHChar_t in_char) ]
// Resize the string so it is in_size characters long, not including the null terminator.
// If in_size is greater than the current size of the string, then the unfilled characters are filled in with in_char.
// The null terminator will still be included automatically.

int8_t CSH_string_add_char(S_CSHString* in_this, CSHChar_t in_char);
CSHChar_t CSH_string_pop_char(S_CSHString* in_this);

int8_t CSH_string_reserve(S_CSHString* in_this, size_t in_size);
int8_t CSH_string_resize(S_CSHString* in_this, size_t in_size, CSHChar_t in_char);
int8_t CSH_string_shrink_to_fit(S_CSHString* in_this);

int8_t CSH_string_insert(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str);
int8_t CSH_string_insert_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str);

// [ int8_t CSH_string_erase(S_CSHString* in_this, size_t in_pos, size_t in_len),
//   int8_t CSH_string_replace_cstr(S_CSHString* in_this, size_t in_pos, size_t in_len, CSHConstCharPtr_t in_str),
//   int8_t CSH_string_replace(S_CSHString* in_this, size_t in_pos, size_t in_len, S_CSHString* in_str), 
//   int8_t CSH_string_copy_arr(S_CSHString* in_this, CSHCharPtr_t in_charArr, size_t in_maxNullSize, size_t in_pos, size_t in_len),
//   S_CSHString CSH_string_substr(S_CSHString* in_this, size_t in_pos, size_t in_len) ]
// in_len, is the length in characters, not including the null terminating character.

int8_t CSH_string_erase(S_CSHString* in_this, size_t in_pos, size_t in_len);
int8_t CSH_string_swap(S_CSHString* in_strOne, S_CSHString* in_strTwo);

int8_t CSH_string_replace(S_CSHString* in_this, size_t in_pos, size_t in_len, S_CSHString* in_str);
int8_t CSH_string_replace_cstr(S_CSHString* in_this, size_t in_pos, size_t in_len, CSHConstCharPtr_t in_str);

// [ int8_t CSH_string_copy_arr(S_CSHString* in_this, CSHCharPtr_t in_charArr, size_t in_maxNullSize, size_t in_pos, size_t in_len) ]
// in_maxNullSize = the maximum number of characters the inputted char array can support, including the null terminating character.

// [ size_t CSH_string_rfind_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str) ]
// Find the last occurance of in_str in a string.

int8_t CSH_string_copy_arr(S_CSHString* in_this, CSHCharPtr_t in_charArr, size_t in_maxNullSize, size_t in_pos, size_t in_len);

size_t CSH_string_find(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str);
size_t CSH_string_find_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str);
size_t CSH_cstr_find(CSHConstCharPtr_t in_strOne, size_t in_maxSize, size_t in_pos, CSHConstCharPtr_t in_strTwo);

size_t CSH_string_rfind(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str);
size_t CSH_string_rfind_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str);
size_t CSH_cstr_rfind(CSHConstCharPtr_t in_strOne, size_t in_maxSize, size_t in_pos, CSHConstCharPtr_t in_strTwo);

S_CSHString CSH_string_substr(S_CSHString* in_this, size_t in_pos, size_t in_len);

int8_t CSH_string_compare(S_CSHString* in_strOne, S_CSHString* in_strTwo);
int8_t CSH_string_compare_cstr(S_CSHString* in_strOne, CSHConstCharPtr_t in_strTwo);
int8_t CSH_cstr_compare(CSHConstCharPtr_t in_strOne, size_t in_maxSize, CSHConstCharPtr_t in_strTwo);

int8_t CSH_string_to_lower(S_CSHString* in_this);
int8_t CSH_string_to_upper(S_CSHString* in_this);

#endif