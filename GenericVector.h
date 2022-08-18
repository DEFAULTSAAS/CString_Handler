#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H
#include <string.h>
#include <stdlib.h>

#define G_VEC_DATA_M(T) S_VecData_##T
#define G_VEC_PUSH_BACK_M(T) vec_push_back_##T
#define G_VEC_POP_BACK_M(T) vec_pop_back_##T
#define G_VEC_CLEAR_M(T) vec_clear_##T
#define G_VEC_SHRINK_TO_FIT_M(T) vec_shrink_to_fit_##T
#define G_VEC_ERASE_M(T) vec_erase_##T
#define G_VEC_RESERVE_M(T) vec_reserve_##T
#define G_VEC_INSERT_M(T) vec_insert_##T

#define G_VEC_DATA_SIZE_M(X) sizeof(X)
#define G_VEC_DATA_DEFAULT_M(T) (G_VEC_DATA_M(T)){NULL, 0, 0, 0}

// Remember to enclose a call to this within an ifndef, define, endif block. See below for an example.
// This to ensure it can be called in header files, without creating multiple defintions in a source file.
//
// #ifndef G_VEC_int
// #define G_VEC_int
// CREATE_GEN_VEC_M(int, int);
// #endif
//
#define CREATE_GEN_VEC_M(X, Y) \
\
typedef struct \
{ \
	X* m_data; \
	size_t m_size; \
	size_t m_capacity; \
	size_t m_dataSize; \
} S_VecData_##Y; \
\
inline void vec_push_back_##Y(S_VecData_##Y * in_vec, X in_data) \
{ \
    if (in_vec->m_capacity < (in_vec->m_size + 1)) \
    { \
        X* tempData = (X*)malloc((in_vec->m_size + 1) * G_VEC_DATA_SIZE_M(X)); \
        \
        if (in_vec->m_size > 0) \
		{ \
        	memcpy(tempData, in_vec->m_data, in_vec->m_size * G_VEC_DATA_SIZE_M(X)); \
            free(in_vec->m_data); \
        } \
		tempData[in_vec->m_size] = in_data; \
		in_vec->m_data = tempData; \
        in_vec->m_size += 1; \
        in_vec->m_capacity += 1; \
    } \
    else \
	{ \
        in_vec->m_data[in_vec->m_size] = in_data; \
		in_vec->m_size += 1; \
    } \
} \
\
inline X vec_pop_back_##Y(S_VecData_##Y * in_vec) \
{ \
    if (in_vec->m_size > 0) \
    { \
        in_vec->m_size -= 1; \
        return in_vec->m_data[in_vec->m_size]; \
    } \
    \
    return *((X*)(~0)); \
} \
\
inline void vec_clear_##Y(S_VecData_##Y * in_vec) \
{ \
	if (in_vec->m_capacity > 0) \
	{ \
		free(in_vec->m_data); \
		in_vec->m_size = 0; \
		in_vec->m_capacity = 0; \
	} \
} \
\
inline void vec_shrink_to_fit_##Y(S_VecData_##Y * in_vec) \
{ \
	if (in_vec->m_size < in_vec->m_capacity) \
	{ \
		X* tempData = (X*)malloc(in_vec->m_size * G_VEC_DATA_SIZE_M(X)); \
		memcpy(tempData, in_vec->m_data, in_vec->m_size * G_VEC_DATA_SIZE_M(X)); \
		free(in_vec->m_data); \
		in_vec->m_data = tempData; \
		in_vec->m_capacity = in_vec->m_size; \
	} \
} \
\
inline void vec_erase_##Y(S_VecData_##Y * in_vec, size_t in_index) \
{ \
	if (in_index < in_vec->m_size) \
	{ \
		for (size_t i = in_index; i < (in_vec->m_size - 1); i++) \
		{ \
			in_vec->m_data[i] = in_vec->m_data[i + 1]; \
		} \
		in_vec->m_size -= 1; \
	} \
} \
\
inline void vec_reserve_##Y(S_VecData_##Y * in_vec, size_t in_size) \
{ \
	if (in_vec->m_capacity < in_size) \
	{ \
		X* tempData = (X*)malloc(in_size * G_VEC_DATA_SIZE_M(X)); \
		memcpy(tempData, in_vec->m_data, in_vec->m_size * G_VEC_DATA_SIZE_M(X)); \
		free(in_vec->m_data); \
		in_vec->m_data = tempData; \
		in_vec->m_capacity = in_size; \
	} \
} \
\
inline void vec_insert_##Y(S_VecData_##Y * in_vec, size_t in_index, X in_data) \
{ \
	if (in_index <= in_vec->m_size) \
	{ \
		if (in_vec->m_capacity < (in_vec->m_size + 1)) \
		{ \
			X* tempData = (X*)malloc((in_vec->m_size + 1) * G_VEC_DATA_SIZE_M(X)); \
			for (size_t i = 0, j = 0; i < (in_vec->m_size + 1); i++) \
			{ \
				if (i == in_index) \
				{ \
					tempData[i] = in_data; \
					continue; \
				} \
				\
				if (i < in_vec->m_size) \
				{ \
					tempData[i] = in_vec->m_data[j]; \
					j += 1; \
				} \
			} \
			\
			free(in_vec->m_data); \
			in_vec->m_data = tempData; \
			in_vec->m_size += 1; \
			in_vec->m_capacity += 1; \
		} \
		else \
		{ \
			X replacedValue; \
			for (size_t i = 0; i < (in_vec->m_size + 1); i++) \
			{ \
				if (i == in_index) \
				{ \
					replacedValue = in_vec->m_data[i]; \
					in_vec->m_data[i] = in_data; \
					continue; \
				} \
				if (i > in_index) \
				{ \
					X tempValue = in_vec->m_data[i]; \
					in_vec->m_data[i] = replacedValue; \
					replacedValue = tempValue; \
				} \
			} \
			in_vec->m_size += 1; \
		} \
	} \
}
	
#endif