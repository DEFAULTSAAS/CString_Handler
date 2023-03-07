#ifndef CSH_GENERAL_UTILS_H
#define CSH_GENERAL_UTILS_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CSH_STRCPY_MF(in_dest, in_destSize, in_src) strcpy_s(in_dest, in_destSize, in_src)
#define CSH_STRNLEN_MF(in_str, in_strsz) strnlen_s(in_str, in_strsz)
#define CSH_STRLEN_NULL_CHECKED_MF(in_str) (in_str != NULL) ? strlen(in_str) : 0
#define CSH_STRNCMP_MF(in_strOne, in_strTwo, in_num) strncmp(in_strOne, in_strTwo, in_num)
#define CSH_FOPEN_MF(in_file, in_fileLoc, in_mode) fopen_s(in_file, in_fileLoc, in_mode)
#define CSH_SPRINTF_MF(in_buffer, in_sizeOfBuffer, ...) sprintf_s(in_buffer, in_sizeOfBuffer, __VA_ARGS__)

#endif