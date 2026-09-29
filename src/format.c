#include <ctype.h>
#include <string.h>

#include "rebus_internal.h"

int rebus_format(const rebus_t *rebus, const char *input,
                 const int digits[MAX_LETTERS], char *out, int size)
{
    if (out == NULL || size <= 0)
        return REBUS_ERR_BUFFER;

    /* пробелы и \n в конце ввода в ответ не переносим */
    int len = (int)strlen(input);
    while (len > 0 && isspace((unsigned char)input[len - 1]))
        len--;

    /* нужно len символов и '\0' */
    if (len + 1 > size)
        return REBUS_ERR_BUFFER;

    for (int i = 0; i < len; i++)
    {
        char c = input[i];
        if (c >= 'A' && c <= 'Z')
            out[i] = (char)('0' + digits[rebus->letter_index[c - 'A']]);
        else
            out[i] = c;
    }
    out[len] = '\0';

    return REBUS_OK;
}
