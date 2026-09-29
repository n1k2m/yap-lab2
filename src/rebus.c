#include <stdlib.h>

#include "rebus_internal.h"

int rebus_solve(const char *input, char *out, int size)
{
    if (out == NULL || size <= 0)
        return REBUS_ERR_BUFFER;

    rebus_t rebus;
    int rc = rebus_parse(input, &rebus);
    if (rc != REBUS_OK)
        return rc;

    int digits[MAX_LETTERS];
    bool found = rebus.has_mul_div ? solve_general(&rebus, digits)
                                   : solve_linear(&rebus, digits);
    if (!found)
        return REBUS_ERR_NO_SOLUTION;

    return rebus_format(&rebus, input, digits, out, size);
}

const char *rebus_strerror(int code)
{
    switch (code)
    {
    case REBUS_OK:              return "ok";
    case REBUS_ERR_PARSE:       return "invalid input";
    case REBUS_ERR_NO_SOLUTION: return "no solution";
    case REBUS_ERR_BUFFER:      return "output buffer too small";
    default:                    return "unknown error";
    }
}
