/* Решение для + и -.
   v2: перебор с возвратом и отсечением. Ребус сведён к
   сумма weight[i] * digit[i] == 0, где weight[i] - вклад буквы i
   (+-10^разряд за каждое вхождение). Цифры назначаются по одной букве:
   - ведущая буква не получает 0 сразу при назначении;
   - частичная сумма накапливается по ходу, а не считается заново;
   - если оставшиеся буквы уже не могут довести сумму до нуля,
     ветка отбрасывается. */

#include "rebus_internal.h"

typedef struct search_t
{
    int n_letters;
    long long weight[MAX_LETTERS];
    /* rest[k] - максимум |вклада| букв k..n-1: сумма |weight[i]| * 9 */
    long long rest[MAX_LETTERS + 1];
    bool is_leading[MAX_LETTERS];
    int digits[MAX_LETTERS];
    /* бит d установлен, если цифра d уже занята */
    unsigned used;
} search_t;

static long long abs_ll(long long x)
{
    return x < 0 ? -x : x;
}

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

/* Назначает цифру букве k и уходит к следующей.
   sum - вклад уже назначенных букв 0..k-1 */
static bool search(search_t *s, int k, long long sum)
{
    if (k == s->n_letters)
        return sum == 0;

    /* оставшиеся буквы изменят сумму не больше чем на rest[k] */
    if (abs_ll(sum) > s->rest[k])
        return false;

    for (int d = s->is_leading[k] ? 1 : 0; d < 10; d++)
    {
        unsigned bit = 1u << d;
        if (s->used & bit)
            continue;

        s->used |= bit;
        s->digits[k] = d;
        if (search(s, k + 1, sum + s->weight[k] * d))
            return true;
        s->used &= ~bit;
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

    for (int i = s.n_letters - 1; i >= 0; i--)
        s.rest[i] = s.rest[i + 1] + abs_ll(s.weight[i]) * 9;

    if (!search(&s, 0, 0))
        return false;

    for (int i = 0; i < MAX_LETTERS; i++)
        digits[i] = s.digits[i];
    return true;
}
