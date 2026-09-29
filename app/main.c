/* lab2 "SEND + MORE = MONEY"  - решить ребус из аргументов;
   lab2 < file_name.txt        - решить из файла */

#include <stdio.h>
#include <string.h>

#include "rebus.h"

#define LINE_SIZE 256

/* Решает один ребус и печатает ответ. Возвращает код из rebus.h. */
static int solve_and_print(const char *input)
{
    char out[LINE_SIZE];
    int rc = rebus_solve(input, out, sizeof(out));

    if (rc == REBUS_OK)
        printf("%s\n", out);
    else
        fprintf(stderr, "error: %s\n", rebus_strerror(rc));
    return rc;
}

/* Проверяет, что в строке есть что-то кроме пробелов */
static int is_blank(const char *s)
{
    return s[strspn(s, " \t\r\n")] == '\0';
}

int main(int argc, char *argv[])
{
    /* Ребус в аргументах: lab2 SEND + MORE = MONEY без кавычек тоже работает */
    if (argc > 1)
    {
        char line[LINE_SIZE] = "";
        for (int i = 1; i < argc; i++)
        {
            if (strlen(line) + strlen(argv[i]) + 2 > sizeof(line))
            {
                fprintf(stderr, "error: input too long\n");
                return 1;
            }
            if (i > 1)
                strcat(line, " ");
            strcat(line, argv[i]);
        }
        return solve_and_print(line) == REBUS_OK ? 0 : 1;
    }

    /* Иначе читаем ребусы построчно */
    char line[LINE_SIZE];
    int failed = 0;
    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        if (is_blank(line))
            continue;
        if (solve_and_print(line) != REBUS_OK)
            failed = 1;
    }
    return failed;
}
