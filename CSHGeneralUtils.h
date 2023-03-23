#ifndef CSH_GENERAL_UTILS_H
#define CSH_GENERAL_UTILS_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

inline size_t CSH_internal_strnlen_s(const char* in_str, size_t in_strSize)
{
	if (in_str == NULL)
	{
		return in_strSize;
	}
	
	void* result = memchr((const void*)in_str, '\0', in_strSize);
	return (result != NULL) ? (size_t)(result - (void*)in_str) : in_strSize;
}

inline int CSH_internal_strcpy_s(char* in_dest, size_t in_destSize, const char* in_src)
{
	if (in_dest == NULL)
	{
		return -1;
	}
	if (in_src == NULL)
	{
		in_dest[0] = '\0';
		return -2;
	}
	
	size_t result = CSH_internal_strnlen_s(in_src, in_destSize);
	
	if (in_destSize == 0 || result >= in_destSize)
	{
		in_dest[0] = '\0';
		return result;
	}
	
	return strcpy(in_dest, in_src);
}

#define CSH_STRCPY_MF(in_dest, in_destSize, in_src) CSH_internal_strcpy_s(in_dest, in_destSize, in_src)
#define CSH_STRNLEN_MF(in_str, in_strSize) CSH_internal_strnlen_s(in_str, in_strSize)
#define CSH_STRLEN_NULL_CHECKED_MF(in_str) (in_str != NULL) ? strlen(in_str) : 0
#define CSH_STRNCMP_MF(in_strOne, in_strTwo, in_num) strncmp(in_strOne, in_strTwo, in_num)
#define CSH_FOPEN_MF(in_file, in_fileLoc, in_mode) fopen_s(in_file, in_fileLoc, in_mode)
#define CSH_SPRINTF_MF(in_buffer, in_sizeOfBuffer, ...) sprintf_s(in_buffer, in_sizeOfBuffer, __VA_ARGS__)

#endif