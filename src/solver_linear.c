/* Решение для + и -.
   v0: простое решение: полный перебор всех размещений цифр по буквам.
   Все проверки делаются только когда цифры назначены всем буквам. */

#include "rebus_internal.h"

typedef struct search_t
{
    const rebus_t *rebus;
    int digits[MAX_LETTERS];
    bool used[10];
} search_t;

/* Собирает число из цифр слова */
static long long word_value(const word *w, const int digits[MAX_LETTERS])
{
    long long value = 0;
    for (int i = 0; i < w->len; i++)
        value = value * 10 + digits[w->letters[i]];
    return value;
}

/* Проверяет полностью назначенный вариант */
static bool check(const search_t *s)
{
    const rebus_t *rebus = s->rebus;

    for (int i = 0; i < rebus->n_letters; i++)
        if (rebus->is_leading[i] && s->digits[i] == 0)
            return false;

    long long sum = 0;
    for (int i = 0; i < rebus->n_operands; i++)
        sum += rebus->operands[i].sign * word_value(&rebus->operands[i], s->digits);

    return sum == word_value(&rebus->result, s->digits);
}

/* Назначает цифру букве k и уходит к следующей */
static bool search(search_t *s, int k)
{
    if (k == s->rebus->n_letters)
        return check(s);

    for (int d = 0; d < 10; d++)
    {
        if (s->used[d])
            continue;

        s->used[d] = true;
        s->digits[k] = d;
        if (search(s, k + 1))
            return true;
        s->used[d] = false;
    }
    return false;
}

bool solve_linear(const rebus_t *rebus, int digits[MAX_LETTERS])
{
    search_t s = {0};
    s.rebus = rebus;

    if (!search(&s, 0))
        return false;

    for (int i = 0; i < MAX_LETTERS; i++)
        digits[i] = s.digits[i];
    return true;
}
