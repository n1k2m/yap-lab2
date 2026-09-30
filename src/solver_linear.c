/* Решение для + и -.
   v1: полный перебор всех размещений цифр по буквам, но проверка варианта
   упрощена предвычислением: ребус сводится к сумма weight[i] * digit[i] == 0,
   где weight[i] - вклад буквы i (+-10^разряд за каждое вхождение).
   Все проверки делаются только когда цифры назначены всем буквам. */

#include "rebus_internal.h"

typedef struct search_t
{
    int n_letters;
    long long weight[MAX_LETTERS];
    bool is_leading[MAX_LETTERS];
    int digits[MAX_LETTERS];
    bool used[10];
} search_t;

/* Добавляет вклад слова в веса букв: sign * 10^разряд за каждую букву */
static void add_word_weights(long long weight[MAX_LETTERS], const word *w, int sign)
{
    long long power = 1;
    for (int i = w->len - 1; i >= 0; i--)
    {
        weight[w->letters[i]] += sign * power;
        power *= 10;
    }
}

/* Проверяет полностью назначенный вариант */
static bool check(const search_t *s)
{
    for (int i = 0; i < s->n_letters; i++)
        if (s->is_leading[i] && s->digits[i] == 0)
            return false;

    long long sum = 0;
    for (int i = 0; i < s->n_letters; i++)
        sum += s->weight[i] * s->digits[i];

    return sum == 0;
}

/* Назначает цифру букве k и уходит к следующей */
static bool search(search_t *s, int k)
{
    if (k == s->n_letters)
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

    /* всё, что не зависит от перебора, считаем один раз */
    s.n_letters = rebus->n_letters;
    for (int i = 0; i < rebus->n_letters; i++)
        s.is_leading[i] = rebus->is_leading[i];
    for (int i = 0; i < rebus->n_operands; i++)
        add_word_weights(s.weight, &rebus->operands[i], rebus->operands[i].sign);
    add_word_weights(s.weight, &rebus->result, -1);

    if (!search(&s, 0))
        return false;

    for (int i = 0; i < MAX_LETTERS; i++)
        digits[i] = s.digits[i];
    return true;
}
