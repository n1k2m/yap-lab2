#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "rebus.h"

#define OUT_SIZE 256

typedef struct
{
    const char *input;
    int size;     /* размер буфера для ответа */
    int expected; /* ожидаемый код возврата */
} test_case;

static const test_case tests[] = {
    /* решаемые ребусы */
    {"SEND + MORE = MONEY",                  OUT_SIZE, REBUS_OK},
    {"BE + BE = MOO",                        OUT_SIZE, REBUS_OK},
    {"BIG + CAT = LION",                     OUT_SIZE, REBUS_OK},
    {"ELEVEN + NINE + FIVE + FIVE = THIRTY", OUT_SIZE, REBUS_OK},
    {"TEN + TEN + FORTY = SIXTY",            OUT_SIZE, REBUS_OK},
    {"A + B = C",                            OUT_SIZE, REBUS_OK},
    {"A + A + A + A + A + A + A = BC",       OUT_SIZE, REBUS_OK},
    {"SEND+MORE=MONEY",                      OUT_SIZE, REBUS_OK},
    {"SEND + MORE = MONEY\n",                OUT_SIZE, REBUS_OK},
    {"MONEY - MORE = SEND",                  OUT_SIZE, REBUS_OK},

    /* умножение и деление */
    {"AB * C = DE",                          OUT_SIZE, REBUS_OK},
    {"TWO * TWO = SQUARE",                   OUT_SIZE, REBUS_OK},
    {"A * BC = A",                           OUT_SIZE, REBUS_OK},
    {"ABC / DE = F",                         OUT_SIZE, REBUS_OK},
    {"SQUARE / TWO = TWO",                   OUT_SIZE, REBUS_OK},
    {"AB / C = D",                           OUT_SIZE, REBUS_OK},

    /* ошибки */
    {"AB + AB = A",                          OUT_SIZE, REBUS_ERR_NO_SOLUTION},
    {"AB * AB = A",                          OUT_SIZE, REBUS_ERR_NO_SOLUTION},
    {"A * B + C = D",                        OUT_SIZE, REBUS_ERR_PARSE},
    {"",                                     OUT_SIZE, REBUS_ERR_PARSE},
    {"send + more = money",                  OUT_SIZE, REBUS_ERR_PARSE},
    {"ABC + DEF = GHIJK",                    OUT_SIZE, REBUS_ERR_PARSE},
    {"A + B + C + D + E + F + G + H = I",    OUT_SIZE, REBUS_ERR_PARSE},
    {"SEND + MORE = MONEY",                  15,       REBUS_ERR_BUFFER},
};

/* Длина строки без пробелов в конце */
static int trimmed_len(const char *s)
{
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        len--;
    return len;
}

/* Проверяет, что out — корректное решение ребуса input */
static bool validate(const char *input, const char *out, const char **why)
{
    int len = trimmed_len(input);
    if ((int)strlen(out) != len)
    {
        *why = "length differs from input";
        return false;
    }

    /* 1. На месте букв цифры, остальное совпадает;
          одна буква — одна цифра, разные буквы — разные цифры */
    int digit_of[26];
    int letter_of[10];
    for (int i = 0; i < 26; i++) digit_of[i] = -1;
    for (int i = 0; i < 10; i++) letter_of[i] = -1;

    for (int i = 0; i < len; i++)
    {
        char c = input[i];
        if (c >= 'A' && c <= 'Z')
        {
            if (!isdigit((unsigned char)out[i]))
            {
                *why = "letter not replaced by digit";
                return false;
            }
            int l = c - 'A';
            int d = out[i] - '0';
            if (digit_of[l] == -1 && letter_of[d] == -1)
            {
                digit_of[l] = d;
                letter_of[d] = l;
            }
            else if (digit_of[l] != d || letter_of[d] != l)
            {
                *why = "letter/digit mapping is not one-to-one";
                return false;
            }
        }
        else if (out[i] != c)
        {
            *why = "non-letter character changed";
            return false;
        }
    }

    /* 2. Разбираем числа и операторы, проверяем ведущие нули */
    long long nums[16];
    char ops[16];
    int n = 0;
    bool after_eq = false;
    long long result = 0;

    const char *p = out;
    while (*p)
    {
        if (isdigit((unsigned char)*p))
        {
            if (p[0] == '0' && isdigit((unsigned char)p[1]))
            {
                *why = "leading zero";
                return false;
            }
            long long v = 0;
            while (isdigit((unsigned char)*p))
                v = v * 10 + (*p++ - '0');
            if (after_eq)
                result = v;
            else
                nums[n++] = v;
        }
        else
        {
            if (*p == '=')
                after_eq = true;
            else if (strchr("+-*/", *p))
                ops[n - 1] = *p;
            p++;
        }
    }

    if (n < 2 || !after_eq)
    {
        *why = "expected at least two operands and '='";
        return false;
    }

    /* 3. Арифметика */
    bool ok;
    if (n == 2 && ops[0] == '*')
        ok = nums[0] * nums[1] == result;
    else if (n == 2 && ops[0] == '/')
        ok = nums[1] != 0 && nums[0] % nums[1] == 0 && nums[0] / nums[1] == result;
    else
    {
        long long sum = nums[0];
        for (int i = 1; i < n; i++)
            sum += ops[i - 1] == '-' ? -nums[i] : nums[i];
        ok = sum == result;
    }
    if (!ok)
    {
        *why = "arithmetic is wrong";
        return false;
    }
    return true;
}

int main(void)
{
    int n_tests = (int)(sizeof(tests) / sizeof(tests[0]));
    int passed = 0;

    for (int t = 0; t < n_tests; t++)
    {
        const test_case *tc = &tests[t];
        char out[OUT_SIZE];
        const char *why = NULL;

        int rc = rebus_solve(tc->input, out, tc->size);
        bool ok = rc == tc->expected;
        if (!ok)
            why = rebus_strerror(rc);
        else if (rc == REBUS_OK)
            ok = validate(tc->input, out, &why);

        if (ok)
            passed++;
        else
            printf("FAIL \"%.*s\": %s\n",
                   trimmed_len(tc->input), tc->input, why);
    }

    printf("passed %d of %d\n", passed, n_tests);
    return passed == n_tests ? 0 : 1;
}
