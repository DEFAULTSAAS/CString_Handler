#include <stdio.h>
#include "CSHString.h"

int main()
{
    S_CSHString tempString = CSH_string_create_cstr("Hello world", 12);
    printf("%s", tempString.m_strPtr);

    return 0;
}