#include <ctype.h>
#include <string.h>

#include "rebus_internal.h"

static bool is_letter(char c)
{
    return c >= 'A' && c <= 'Z';
}

/* Пропускает пробелы (а также \t, \r, \n) */
static const char *skip_spaces(const char *p)
{
    while (isspace((unsigned char)*p))
        p++;
    return p;
}

/* Возвращает номер буквы c в ребусе. Новой букве выдаёт следующий номер.
   -1, если это была бы (MAX_LETTERS + 1)-я различная буква. */
static int get_letter(rebus_t *rebus, char c)
{
    int idx = rebus->letter_index[c - 'A'];
    if (idx != -1)
        return idx;

    if (rebus->n_letters == MAX_LETTERS)
        return -1;

    idx = rebus->n_letters++;
    rebus->letter_index[c - 'A'] = idx;
    rebus->letter_char[idx] = c;
    return idx;
}

/* Читает слово из букв A-Z начиная с p в w.
   Возвращает указатель на символ после слова или NULL при ошибке. */
static const char *read_word(rebus_t *rebus, const char *p, word *w, int sign)
{
    w->len = 0;
    w->sign = sign;

    if (!is_letter(*p))
        return NULL;

    while (is_letter(*p))
    {
        if (w->len == MAX_WORD_LEN)
            return NULL;

        int idx = get_letter(rebus, *p);
        if (idx == -1)
            return NULL;

        w->letters[w->len++] = idx;
        p++;
    }

    if (w->len > 1)
        rebus->is_leading[w->letters[0]] = true;

    return p;
}

int rebus_parse(const char *input, rebus_t *rebus)
{
    if (input == NULL || rebus == NULL)
        return REBUS_ERR_PARSE;

    memset(rebus, 0, sizeof(*rebus));
    for (int i = 0; i < 26; i++)
        rebus->letter_index[i] = -1;

    const char *p = input;
    int sign = +1;

    /* Левая часть: слово, затем оператор или '=' */
    for (;;)
    {
        p = skip_spaces(p);
        p = read_word(rebus, p, &rebus->operands[rebus->n_operands], sign);
        if (p == NULL)
            return REBUS_ERR_PARSE;
        rebus->n_operands++;

        p = skip_spaces(p);
        if (*p == '=')
        {
            p++;
            break;
        }

        operations op;
        switch (*p)
        {
        case '+': op = ADD; sign = +1; break;
        case '-': op = SUB; sign = -1; break;
        case '*': op = MUL; sign = +1; break;
        case '/': op = DIV; sign = +1; break;
        default:
            return REBUS_ERR_PARSE;
        }

        /* после оператора будет ещё один операнд, а места нет */
        if (rebus->n_operands == MAX_OPERANDS)
            return REBUS_ERR_PARSE;

        rebus->ops[rebus->n_operands - 1] = op;
        p++;
    }

    /* Правая часть: одно слово и конец строки */
    p = skip_spaces(p);
    p = read_word(rebus, p, &rebus->result, +1);
    if (p == NULL)
        return REBUS_ERR_PARSE;

    p = skip_spaces(p);
    if (*p != '\0')
        return REBUS_ERR_PARSE;

    if (rebus->n_operands < 2)
        return REBUS_ERR_PARSE;

    for (int i = 0; i < rebus->n_operands - 1; i++)
        if (rebus->ops[i] == MUL || rebus->ops[i] == DIV)
            rebus->has_mul_div = true;

    /* * и / только в виде A op B = C */
    if (rebus->has_mul_div && rebus->n_operands != 2)
        return REBUS_ERR_PARSE;

    return REBUS_OK;
}
