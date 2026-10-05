/* Решение для + и -.
   v3: перебор с возвратом и отсечением + эвристики. Ребус сведён к
   сумма weight[i] * digit[i] == 0, где weight[i] - вклад буквы i
   (+-10^разряд за каждое вхождение). Цифры назначаются по одной букве:
   - ведущая буква не получает 0 сразу при назначении;
   - частичная сумма накапливается по ходу, а не считается заново;
   - если оставшиеся буквы уже не могут довести сумму до нуля,
     ветка отбрасывается.
   Эвристики:
   - буквы назначаются по убыванию |weight|: старшие разряды фиксируются
     первыми, и отсечение срабатывает раньше;
   - если результат длиннее всех слагаемых, его первая буква не больше
     (количество слагаемых - 1). */

#include "rebus_internal.h"

/* Все массивы, кроме digits, идут в порядке назначения букв:
   k-й назначается буква с номером letter[k] */
typedef struct search_t
{
    int n_letters;
    int letter[MAX_LETTERS];
    long long weight[MAX_LETTERS];
    /* rest[k] - максимум |вклада| букв k..n-1: сумма |weight[i]| * 9 */
    long long rest[MAX_LETTERS + 1];
    /* допустимые цифры буквы: min_digit[k]..max_digit[k] */
    int min_digit[MAX_LETTERS];
    int max_digit[MAX_LETTERS];
    /* digits[i] - цифра буквы с номером i (как в rebus_t) */
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

/* Наибольшая возможная цифра первой буквы результата: если результат
   длиннее всех слагаемых, она равна переносу, а перенос от n слагаемых
   не больше n - 1. Иначе (или если есть вычитание) ограничения нет. */
static int result_first_max(const rebus_t *rebus)
{
    for (int i = 0; i < rebus->n_operands; i++)
        if (rebus->operands[i].sign < 0 || rebus->operands[i].len >= rebus->result.len)
            return 9;
    return rebus->n_operands - 1;
}

/* Назначает цифру k-й по порядку букве и уходит к следующей.
   sum - вклад уже назначенных букв */
static bool search(search_t *s, int k, long long sum)
{
    if (k == s->n_letters)
        return sum == 0;

    /* оставшиеся буквы изменят сумму не больше чем на rest[k] */
    if (abs_ll(sum) > s->rest[k])
        return false;

    for (int d = s->min_digit[k]; d <= s->max_digit[k]; d++)
    {
        unsigned bit = 1u << d;
        if (s->used & bit)
            continue;

        s->used |= bit;
        s->digits[s->letter[k]] = d;
        if (search(s, k + 1, sum + s->weight[k] * d))
            return true;
        s->used &= ~bit;
    }
    return false;
}

bool solve_linear(const rebus_t *rebus, int digits[MAX_LETTERS])
{
    search_t s = {0};
    int n = rebus->n_letters;

    /* всё, что не зависит от перебора, считаем один раз */
    long long weight[MAX_LETTERS] = {0};
    for (int i = 0; i < rebus->n_operands; i++)
        add_word_weights(weight, &rebus->operands[i], rebus->operands[i].sign);
    add_word_weights(weight, &rebus->result, -1);

    /* порядок назначения: по убыванию |weight| (сортировка вставками) */
    s.n_letters = n;
    for (int i = 0; i < n; i++)
    {
        int k = i;
        while (k > 0 && abs_ll(weight[s.letter[k - 1]]) < abs_ll(weight[i]))
        {
            s.letter[k] = s.letter[k - 1];
            k--;
        }
        s.letter[k] = i;
    }

    int first = rebus->result.letters[0];
    int first_max = result_first_max(rebus);
    for (int k = 0; k < n; k++)
    {
        int i = s.letter[k];
        s.weight[k] = weight[i];
        s.min_digit[k] = rebus->is_leading[i] ? 1 : 0;
        s.max_digit[k] = i == first ? first_max : 9;
    }

    for (int k = n - 1; k >= 0; k--)
        s.rest[k] = s.rest[k + 1] + abs_ll(s.weight[k]) * 9;

    if (!search(&s, 0, 0))
        return false;

    for (int i = 0; i < MAX_LETTERS; i++)
        digits[i] = s.digits[i];
    return true;
}
