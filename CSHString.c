#include "CSHString.h"
#include "CSHGeneralUtils.h"
#include <assert.h>

const size_t CSH_STRING_NPOS = ~(0);
const size_t CSH_CHAR_SIZE = sizeof(CSHChar_t);
// 2048 - 1, to account for the extra null terminating character, as the string should fit within 2KB of ram.
const size_t CSH_STRING_MAX_STACK_CHAR_COUNT = ((2048 / CSH_CHAR_SIZE) - 1);

S_CSHString CSH_string_create_cstr(CSHConstCharPtr_t in_str, size_t in_maxSize)
{
    size_t result = CSH_STRNLEN_MF(in_str, (in_maxSize + 1));
    
    if (result == (in_maxSize + 1) || (result == 0 && in_str == NULL))
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_STR);
    }

    S_CSHString tempStr = CSH_STRING_DEFAULT_M;
    tempStr.m_status = CSHSSC_NONE; 
    tempStr.m_size = result;
    tempStr.m_nullSize = result + 1;
    tempStr.m_capacity = tempStr.m_nullSize;
    tempStr.m_maxCstrSize = (in_maxSize + 1); // We do the inputted max cstr size + 1, to account for the null terminating character.
    tempStr.m_strPtr = (CSHCharPtr_t)calloc(tempStr.m_capacity,CSH_CHAR_SIZE);

    #if CSH_STRING_ASSERT_ENABLED_M
        assert(tempStr.m_strPtr != NULL);
    #endif
    
    CSH_STRCPY_MF(tempStr.m_strPtr, tempStr.m_capacity, in_str);
    return tempStr;   
}

S_CSHString CSH_string_create(S_CSHString* in_str)
{
    if (in_str == NULL)
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_STR);
    }

    S_CSHString tempStr = CSH_STRING_DEFAULT_M;

    if (in_str->m_capacity != 0)
    {
        tempStr.m_capacity = in_str->m_capacity;
        tempStr.m_strPtr = (CSHCharPtr_t)calloc(tempStr.m_capacity, CSH_CHAR_SIZE);

        #if CSH_STRING_ASSERT_ENABLED_M
            assert(tempStr.m_strPtr != NULL);
        #endif
    }

    if (in_str->m_nullSize != 0)
    {
        CSH_STRCPY_MF(tempStr.m_strPtr, tempStr.m_capacity, in_str->m_strPtr);
    }
    tempStr.m_status = in_str->m_status;
    tempStr.m_size = in_str->m_size;
    tempStr.m_nullSize = in_str->m_nullSize;
    tempStr.m_maxCstrSize = in_str->m_maxCstrSize;

    return tempStr;
}

int8_t CSH_string_free(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_strPtr != NULL && in_this->m_status != CSHSSC_USE_ALLOCA)
    {
        in_this->m_size = 0;
        in_this->m_nullSize = 0;
        in_this->m_capacity = 0;

        free(in_this->m_strPtr);
        in_this->m_strPtr = NULL;
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_clear(S_CSHString* in_this, bool in_freeMemory)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    if (in_this->m_size > 0)
    {
        in_this->m_strPtr[0] = '\0';
        in_this->m_size = 0;
        in_this->m_nullSize = 1;
    }
    if (in_freeMemory)
    {
        return CSH_string_free(in_this);
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_set_max_cstr_size(S_CSHString* in_this, size_t in_maxSize)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    in_this->m_maxCstrSize = (in_maxSize + 1);
    return CSHSSC_NONE;
}

int8_t CSH_string_cstr_fit(S_CSHString* in_this, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    size_t result = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (result == in_this->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    return CSHSSC_NONE;
}

size_t CSH_cstr_size(CSHConstCharPtr_t in_str, size_t in_maxSize)
{
    if (in_str == NULL)
    {
        return CSH_STRING_NPOS;
    }

    size_t result = CSH_STRNLEN_MF(in_str, (in_maxSize + 1));
    if (result == (in_maxSize + 1))
    {
        return CSH_STRING_NPOS;
    }

    return result; 
}

size_t CSH_string_size_bytes(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSH_STRING_NPOS;
    }

    return (in_this->m_size * CSH_CHAR_SIZE);
}

size_t CSH_string_null_size_bytes(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSH_STRING_NPOS;
    }

    return (in_this->m_nullSize * CSH_CHAR_SIZE);
}

int8_t CSH_string_assign_cstr(S_CSHString* in_this, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (CSH_string_cstr_fit(in_this, in_str) < 0)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    size_t cstrLen = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (cstrLen < in_this->m_capacity)
    {
        CSH_STRCPY_MF(in_this->m_strPtr, in_this->m_capacity, in_str);
        in_this->m_size = cstrLen;
        in_this->m_nullSize = cstrLen + 1;

        return CSHSSC_NONE;
    }

    S_CSHString tempStr = {NULL, in_this->m_status, 0, 0, 0, in_this->m_maxCstrSize};
    CSH_string_free(in_this);
    *in_this = CSH_string_create_cstr(in_str, (tempStr.m_maxCstrSize - 1));
    in_this->m_status = tempStr.m_status;
    in_this->m_maxCstrSize = in_this->m_maxCstrSize;

    return CSHSSC_NONE;
}

int8_t CSH_string_assign(S_CSHString* in_this, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    if (in_this->m_size < in_this->m_capacity)
    {
        CSH_STRCPY_MF(in_this->m_strPtr, in_this->m_capacity, in_str->m_strPtr);
        in_this->m_size = in_str->m_size;
        in_this->m_nullSize = in_str->m_nullSize;

        return CSHSSC_NONE;
    }
    
    CSH_string_free(in_this);
    *in_this = CSH_string_create(in_str);

    return CSHSSC_NONE;
}

S_CSHString CSH_string_create_concat_cstr(CSHConstCharPtr_t in_strOne, CSHConstCharPtr_t in_strTwo, size_t in_maxSizeOne, size_t in_maxSizeTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_STR);
    }
    size_t resultOne = CSH_STRNLEN_MF(in_strOne, (in_maxSizeOne + 1));
    size_t resultTwo = CSH_STRNLEN_MF(in_strTwo, (in_maxSizeTwo + 1));

    if (resultOne == (in_maxSizeOne + 1) || resultTwo == (in_maxSizeTwo + 1))
    {
        return CSH_STRING_ERROR_M(CSHSSC_CSTR_DOESNT_FIT);
    }

    S_CSHString tempStr = CSH_STRING_DEFAULT_M;
    tempStr.m_status = CSHSSC_NONE;
    tempStr.m_size = resultOne + resultTwo;
    tempStr.m_nullSize = tempStr.m_size + 1;
    tempStr.m_capacity = tempStr.m_nullSize;
    // If the average of the two maximum sizes is greater than CSH_STRING_MAX_CSTR_CHAR_COUNT_M, then set maxCstrSize to that average, 
    // otherwise just set it to CSH_STRING_MAX_CSTR_CHAR_COUNT_M.
    tempStr.m_maxCstrSize = (((in_maxSizeOne + in_maxSizeTwo) / 2) > CSH_STRING_MAX_CSTR_CHAR_COUNT_M) ? (((in_maxSizeOne + in_maxSizeTwo) / 2) + 1) : (CSH_STRING_MAX_CSTR_CHAR_COUNT_M + 1);
    tempStr.m_strPtr = (CSHCharPtr_t)calloc(tempStr.m_capacity, CSH_CHAR_SIZE);

    #if CSH_STRING_ASSERT_ENABLED_M
        assert(tempStr.m_strPtr != NULL);
    #endif

    for (size_t i = 0; i < resultOne; i++)
    {
        tempStr.m_strPtr[i] = in_strOne[i];
    }
    for (size_t i = 0; i < resultTwo; i++)
    {
        tempStr.m_strPtr[i + resultOne] = in_strTwo[i];
    }

    return tempStr; 
}

S_CSHString CSH_string_create_concat_left_cstr(CSHConstCharPtr_t in_strOne, S_CSHString* in_strTwo)
{
    return CSH_string_create_concat_cstr(in_strOne, in_strTwo->m_strPtr, (in_strTwo->m_maxCstrSize - 1), in_strTwo->m_size);
}

S_CSHString CSH_string_create_concat_right_cstr(S_CSHString* in_strOne, CSHConstCharPtr_t in_strTwo)
{
    return CSH_string_create_concat_cstr(in_strOne->m_strPtr, in_strTwo, in_strOne->m_size, (in_strOne->m_maxCstrSize - 1));
}

S_CSHString CSH_string_create_concat(S_CSHString* in_strOne, S_CSHString* in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_STR);
    }

    S_CSHString tempStr = CSH_STRING_DEFAULT_M;
    tempStr.m_status = CSHSSC_NONE;
    tempStr.m_size = in_strOne->m_size + in_strTwo->m_size;
    tempStr.m_nullSize = tempStr.m_size + 1;
    tempStr.m_capacity = tempStr.m_nullSize;
    tempStr.m_maxCstrSize = ((tempStr.m_size / 2) > CSH_STRING_MAX_CSTR_CHAR_COUNT_M) ? ((tempStr.m_size / 2) + 1) : (CSH_STRING_MAX_CSTR_CHAR_COUNT_M + 1);
    tempStr.m_strPtr = (CSHCharPtr_t)calloc(tempStr.m_size, CSH_CHAR_SIZE);

    #if CSH_STRING_ASSERT_ENABLED_M
        assert(tempStr.m_strPtr != NULL);
    #endif

    for (size_t i = 0; i < in_strOne->m_size; i++)
    {
        tempStr.m_strPtr[i] = in_strOne->m_strPtr[i];
    }
    for (size_t i = 0; i < in_strTwo->m_size; i++)
    {
        tempStr.m_strPtr[i + in_strOne->m_size] = in_strTwo->m_strPtr[i];
    }

    return tempStr;
}

int8_t CSH_string_concat_left_cstr(CSHConstCharPtr_t in_str, S_CSHString* in_this)
{
    if (in_str == NULL || in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    size_t result = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (result == in_this->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    in_this->m_size = in_this->m_size + result;
    in_this->m_nullSize = in_this->m_size + 1;

    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    for (intmax_t i = (in_this->m_size - 1); i >= 0; i--)
    {
        // Shift the current contents of the string to the right.
        if (in_this->m_strPtr[i] != '\0')
        {
            in_this->m_strPtr[i + result] = in_this->m_strPtr[i];
        }
    }
    for (size_t i = 0; i < result; i++)
    {
        in_this->m_strPtr[i] = in_str[i];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_concat_right_cstr(S_CSHString* in_this, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    size_t result = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (result == in_this->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    size_t originalSize = in_this->m_size;
    in_this->m_size = in_this->m_size + result;
    in_this->m_nullSize = in_this->m_size + 1;
    
    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }
    for (size_t i = 0; i < result; i++)
    {
        in_this->m_strPtr[i + originalSize] = in_str[i];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_concat_left(S_CSHString* in_str, S_CSHString* in_this)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    in_this->m_size += in_str->m_size;
    in_this->m_nullSize = in_this->m_size + 1;
    
    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    for (intmax_t i = (in_this->m_size - 1); i >= 0; i--)
    {
        // Shift the current contents of the string to the right.
        if (in_this->m_strPtr[i] != '\0')
        {
            in_this->m_strPtr[i + in_str->m_size] = in_this->m_strPtr[i];
        }
    }
    for (size_t i = 0; i < in_str->m_size; i++)
    {
        in_this->m_strPtr[i] = in_str->m_strPtr[i];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_concat_right(S_CSHString* in_this, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    size_t originalSize = in_this->m_size;
    in_this->m_size += in_str->m_size;
    in_this->m_nullSize = in_this->m_size + 1;
    
    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }
    for (size_t i = 0; i < in_str->m_size; i++)
    {
        in_this->m_strPtr[i + originalSize] = in_str->m_strPtr[i];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_add_char(S_CSHString* in_this, CSHChar_t in_char)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    if ((in_this->m_size + 1) < in_this->m_capacity)
    {
        in_this->m_strPtr[in_this->m_size] = in_char;
        in_this->m_size += 1;
        in_this->m_nullSize += 1;
        in_this->m_strPtr[in_this->m_size] = '\0';

        return CSHSSC_NONE;
    }

    CSH_string_reserve(in_this, (in_this->m_nullSize + 1));

    in_this->m_strPtr[in_this->m_size] = in_char;
    in_this->m_size += 1;
    in_this->m_nullSize += 1;
    in_this->m_strPtr[in_this->m_size] = '\0';

    return CSHSSC_NONE;
}

CSHChar_t CSH_string_pop_char(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return '\0';
    }

    if (in_this->m_size > 0)
    {
        CSHChar_t tempChar = in_this->m_strPtr[(in_this->m_size - 1)];
        in_this->m_strPtr[(in_this->m_size - 1)] = '\0';
        in_this->m_size -= 1;
        in_this->m_nullSize -= 1;

        return tempChar;
    }

    return '\0';
}

int8_t CSH_string_reserve(S_CSHString* in_this, size_t in_size)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_capacity >= (in_size + 1))
    {
        return CSHSSC_ALREADY_RESERVED;
    }

    if (in_this->m_size == 0)
    {
        if (in_this->m_strPtr != NULL)
        {
            free(in_this->m_strPtr);
            in_this->m_strPtr = NULL;
        }
        
        in_this->m_capacity = (in_size + 1);
        in_this->m_strPtr = (CSHCharPtr_t)calloc(in_this->m_capacity, CSH_CHAR_SIZE);

        #if CSH_STRING_ASSERT_ENABLED_M
            assert(in_this->m_strPtr != NULL);
        #endif
    }
    else 
    {
        S_CSHString tempStr = *in_this;
        tempStr.m_strPtr = NULL;
        tempStr.m_status = CSHSSC_USE_ALLOCA;
        CSH_STRING_CALLOC_MF(tempStr.m_strPtr, tempStr.m_nullSize, CSH_CHAR_SIZE, &tempStr.m_status, false);
        
        #if CSH_STRING_ASSERT_ENABLED_M
            assert(tempStr.m_strPtr != NULL);
        #endif
        CSH_string_assign(&tempStr, in_this);

        free(in_this->m_strPtr);
        in_this->m_capacity = (in_size + 1);
        in_this->m_strPtr = (CSHCharPtr_t)calloc(in_this->m_capacity, CSH_CHAR_SIZE);

        #if CSH_STRING_ASSERT_ENABLED_M
            assert(in_this->m_strPtr != NULL);
        #endif

        for (size_t i = 0; i < tempStr.m_size; i++)
        {
            in_this->m_strPtr[i] = tempStr.m_strPtr[i];
        }
        CSH_string_free(&tempStr);
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_shrink_to_fit(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_nullSize == in_this->m_capacity)
    {
        return CSHSSC_ALREADY_RESERVED;
    }

    S_CSHString tempStr = *in_this;
    tempStr.m_strPtr = NULL;
    tempStr.m_status = CSHSSC_USE_ALLOCA;
    CSH_STRING_CALLOC_MF(tempStr.m_strPtr, tempStr.m_nullSize, CSH_CHAR_SIZE, &tempStr.m_status, false);
    
    #if CSH_STRING_ASSERT_ENABLED_M
        assert(tempStr.m_strPtr != NULL);
    #endif
    CSH_string_assign(&tempStr, in_this);

    free(in_this->m_strPtr);
    in_this->m_strPtr = (CSHCharPtr_t)calloc(in_this->m_nullSize, CSH_CHAR_SIZE);
    in_this->m_capacity = in_this->m_nullSize;

    #if CSH_STRING_ASSERT_ENABLED_M
        assert(in_this->m_strPtr != NULL);
    #endif

    for (size_t i = 0; i < in_this->m_size; i++)
    {
        in_this->m_strPtr[i] = tempStr.m_strPtr[i];
    }
    CSH_string_free(&tempStr);

    return CSHSSC_NONE;   
}

int8_t CSH_string_resize(S_CSHString* in_this, size_t in_size, CSHChar_t in_char)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_size == in_size)
    {
        return CSHSSC_ALREADY_RESERVED;
    }

    if (in_this->m_size > in_size)
    {
        in_this->m_strPtr[in_size] = '\0';
        in_this->m_size = in_size;
        in_this->m_nullSize = in_this->m_size + 1;

        return CSHSSC_NONE;
    }

    CSH_string_reserve(in_this, in_size + 1);
    for (size_t i = in_this->m_size; i < in_size; i++)
    {
        in_this->m_strPtr[i] = in_char;    
    }
    if (in_char != '\0')
    {
        in_this->m_size = in_size;
        in_this->m_nullSize = in_this->m_size + 1;
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_insert_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }

    size_t result = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (result == in_this->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    in_this->m_size += result;
    in_this->m_nullSize = in_this->m_size + 1;

    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    // (in_this->m_size - 1) to account for in_this->m_strPtr[i + 1], which ensures the character at in_pos is replaced with the first character of in_str.
    for (size_t i = in_pos; i < (in_this->m_size - 1); i++)
    {
        in_this->m_strPtr[i + result] = in_this->m_strPtr[i + 1];
    }
    for (size_t i = 0; i < result; i++)
    {
        in_this->m_strPtr[in_pos + i] = in_str[i];
    }

    return CSHSSC_NONE;  
}

int8_t CSH_string_insert(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }

    in_this->m_size += in_str->m_size;
    in_this->m_nullSize = in_str->m_size + 1;

    if (in_this->m_capacity < in_this->m_nullSize)
    {
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    // (in_this->m_size - 1) to account for in_this->m_strPtr[i + 1], which ensures the character at in_pos is replaced with the first character of in_str.
    for (size_t i = in_pos; i < (in_this->m_size - 1); i++)
    {
        in_this->m_strPtr[i + in_str->m_size] = in_this->m_strPtr[i + 1];
    }
    for (size_t i = 0; i < in_str->m_size; i++)
    {
        in_this->m_strPtr[in_pos + i] = in_str->m_strPtr[i];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_erase(S_CSHString* in_this, size_t in_pos, size_t in_len)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }
    if (in_len == 0)
    {
        return CSHSSC_NONE;
    }

    if ((in_pos + in_len) > in_this->m_size)
    {
        in_len = (in_this->m_size - in_pos);
    }

    size_t endSubStrPos = (in_pos + in_len);
    S_CSHString tempStr = CSH_STRING_DEFAULT_M;
    CSH_string_reserve(&tempStr, in_this->m_size - endSubStrPos);

    for (size_t i = endSubStrPos, counter = 0; i < in_this->m_size; i++, counter++)
    {
        CSH_string_add_char(&tempStr, in_this->m_strPtr[i]);
    }
    for (size_t i = in_pos, counter = 0; counter < tempStr.m_size; i++, counter++)
    {
        in_this->m_strPtr[i] = tempStr.m_strPtr[counter];
    }

    in_this->m_size -= in_len;
    in_this->m_nullSize = in_this->m_size + 1;
    in_this->m_strPtr[in_this->m_size] = '\0';
    
    CSH_string_free(&tempStr);

    return CSHSSC_NONE;
}

int8_t CSH_string_swap(S_CSHString* in_strOne, S_CSHString* in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    S_CSHString tempStr = *in_strOne;
    *in_strOne = *in_strTwo;
    *in_strTwo = tempStr;

    return CSHSSC_NONE;
}

int8_t CSH_string_replace_cstr(S_CSHString* in_this, size_t in_pos, size_t in_len, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }
    if (in_len == 0)
    {
        return CSHSSC_NONE;
    }

    size_t result = CSH_STRNLEN_MF(in_str, in_this->m_maxCstrSize);
    if (result < in_len)
    {
        in_len = result;
    }
    if (in_len >= in_this->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    intmax_t replaceSize = (in_this->m_size - (in_pos + in_len));
    if (replaceSize > 0)
    {
        in_this->m_size += replaceSize;
        in_this->m_nullSize = in_this->m_size + 1;
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    for (size_t i = in_pos, counter = 0; counter < in_len; i++, counter++)
    {
        in_this->m_strPtr[i] = in_str[counter];
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_replace(S_CSHString* in_this, size_t in_pos, size_t in_len, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }
    if (in_len == 0)
    {
        return CSHSSC_NONE;
    }

    if (in_this->m_size < in_len)
    {
        in_len = in_this->m_size;
    }

    intmax_t replaceSize = (in_this->m_size - (in_pos + in_len));
    if (replaceSize > 0)
    {
        in_this->m_size += replaceSize;
        in_this->m_nullSize = in_this->m_size + 1;
        CSH_string_reserve(in_this, in_this->m_nullSize);
    }

    for (size_t i = in_pos, counter = 0; counter < in_len; i++, counter++)
    {
        in_this->m_strPtr[i] = in_str->m_strPtr[counter];
    }

    return CSHSSC_NONE;   
}

int8_t CSH_string_copy_arr(S_CSHString* in_this, CSHCharPtr_t in_charArr, size_t in_maxNullSize, size_t in_pos, size_t in_len)
{
    if (in_this == NULL || in_charArr == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }
    if (in_len == 0)
    {
        return CSHSSC_NONE;
    }

    if (in_this->m_size < in_len)
    {
        in_len = in_this->m_size;
    }
    if (in_len > (in_maxNullSize - 1))
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }

    for (size_t i = in_pos, counter = 0; counter < in_len; i++, counter++)
    {
        in_charArr[counter] = in_this->m_strPtr[i]; 
    }
    in_charArr[in_len] = '\0';

    return CSHSSC_NONE;
}

size_t CSH_string_find_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSH_STRING_NPOS;
    }
    if (CSH_string_cstr_fit(in_this, in_str) < 0)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_this->m_strPtr + in_pos), in_str);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    return (in_pos + (tempCharPtr - (in_this->m_strPtr + in_pos)));
}

size_t CSH_cstr_find(CSHConstCharPtr_t in_strOne, size_t in_maxSize, size_t in_pos, CSHConstCharPtr_t in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= CSH_STRNLEN_MF(in_strOne, (in_maxSize + 1)))
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_strOne + in_pos), in_strTwo);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    return (in_pos + (tempCharPtr - (in_strOne + in_pos)));
}

size_t CSH_string_find(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_this->m_strPtr + in_pos), in_str->m_strPtr);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    return (in_pos + (tempCharPtr - (in_this->m_strPtr + in_pos)));
}

size_t CSH_string_rfind_cstr(S_CSHString* in_this, size_t in_pos, CSHConstCharPtr_t in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSH_STRING_NPOS;
    }
    if (CSH_string_cstr_fit(in_this, in_str) < 0)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_this->m_strPtr + in_pos), in_str);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t lastCharPtr = tempCharPtr;
    while (true)
    {
        tempCharPtr = strstr(lastCharPtr, in_str);
        if (tempCharPtr != NULL)
        {
            lastCharPtr = tempCharPtr;
            continue;
        }

        break;
    }

    return (in_pos + (lastCharPtr - (in_this->m_strPtr + in_pos)));
}

size_t CSH_cstr_rfind(CSHConstCharPtr_t in_strOne, size_t in_maxSize, size_t in_pos, CSHConstCharPtr_t in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= CSH_STRNLEN_MF(in_strOne, (in_maxSize + 1)))
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_strOne + in_pos), in_strTwo);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t lastCharPtr = tempCharPtr;
    while (true)
    {
        tempCharPtr = strstr(lastCharPtr, in_strTwo);
        if (tempCharPtr != NULL)
        {
            lastCharPtr = tempCharPtr;
            continue;
        }

        break;
    }

    return (in_pos + (lastCharPtr - (in_strOne + in_pos)));
}

size_t CSH_string_rfind(S_CSHString* in_this, size_t in_pos, S_CSHString* in_str)
{
    if (in_this == NULL || in_str == NULL)
    {
        return CSH_STRING_NPOS;
    }
    if (in_pos >= in_this->m_size)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t tempCharPtr = strstr((in_this->m_strPtr + in_pos), in_str->m_strPtr);
    if (tempCharPtr == NULL)
    {
        return CSH_STRING_NPOS;
    }

    CSHCharPtr_t lastCharPtr = tempCharPtr;
    while (true)
    {
        tempCharPtr = strstr(lastCharPtr, in_str->m_strPtr);
        if (tempCharPtr != NULL)
        {
            lastCharPtr = tempCharPtr;
            continue;
        }

        break;
    }

    return (in_pos + (lastCharPtr - (in_this->m_strPtr + in_pos)));
}

S_CSHString CSH_string_substr(S_CSHString* in_this, size_t in_pos, size_t in_len)
{
    if (in_this == NULL)
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_STR);
    }
    if (in_pos >= in_this->m_size)
    {
        return CSH_STRING_ERROR_M(CSHSSC_BAD_INPUT_ARG);
    }
    if (in_len == 0)
    {
        return CSH_STRING_DEFAULT_M;
    }

    S_CSHString tempStr = CSH_STRING_DEFAULT_M;
    CSH_string_reserve(&tempStr, (in_len + 1));
    tempStr.m_size = in_len;
    tempStr.m_nullSize = (in_len + 1);
    tempStr.m_capacity = tempStr.m_nullSize;

    for (size_t i = in_pos, counter = 0; counter < tempStr.m_size; i++, counter++)
    {
        tempStr.m_strPtr[counter] = in_this->m_strPtr[i];
    }

    return tempStr;
}

int8_t CSH_string_compare_cstr(S_CSHString* in_strOne, CSHConstCharPtr_t in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    
    size_t result = CSH_STRNLEN_MF(in_strTwo, in_strOne->m_maxCstrSize);
    if (result == in_strOne->m_maxCstrSize)
    {
        return CSHSSC_CSTR_DOESNT_FIT;
    }
    if (in_strOne->m_size != result)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }

    for (size_t i = 0; i < result; i++)
    {
        if (in_strOne->m_strPtr[i] != in_strTwo[i])
        {
            return CSHSSC_BAD_INPUT_ARG;
        }
    }

    return CSHSSC_NONE;
}

int8_t CSH_cstr_compare(CSHConstCharPtr_t in_strOne, size_t in_maxSize, CSHConstCharPtr_t in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }

    int result = CSH_STRNCMP_MF(in_strOne, in_strTwo, in_maxSize);
    if (result != 0)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_compare(S_CSHString* in_strOne, S_CSHString* in_strTwo)
{
    if (in_strOne == NULL || in_strTwo == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_strOne->m_size != in_strTwo->m_size)
    {
        return CSHSSC_BAD_INPUT_ARG;
    }

    for (size_t i = 0; i < in_strOne->m_size; i++)
    {
        if (in_strOne->m_strPtr[i] != in_strTwo->m_strPtr[i])
        {
            return CSHSSC_BAD_INPUT_ARG;
        }
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_to_lower(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_size == 0)
    {
        return CSHSSC_NONE;
    }

    for (size_t i = 0; i < in_this->m_size; i++)
    {
        if ((int)in_this->m_strPtr[i] >= 65 && (int)in_this->m_strPtr[i] <= 90)
        {
            in_this->m_strPtr[i] = (CSHChar_t)((int)in_this->m_strPtr[i] + 32);
        }
    }

    return CSHSSC_NONE;
}

int8_t CSH_string_to_upper(S_CSHString* in_this)
{
    if (in_this == NULL)
    {
        return CSHSSC_BAD_INPUT_STR;
    }
    if (in_this->m_size == 0)
    {
        return CSHSSC_NONE;
    }

    for (size_t i = 0; i < in_this->m_size; i++)
    {
        if ((int)in_this->m_strPtr[i] >= 97 && (int)in_this->m_strPtr[i] <= 122)
        {
            in_this->m_strPtr[i] = (CSHChar_t)((int)in_this->m_strPtr[i] - 32);
        }
    }

    return CSHSSC_NONE;
}