/* Решение для * и / (дополнительное задание).
   Оба случая сводятся к x * y = z:
     A * B = C  ->  x = A, y = B, z = C;
     A / B = C  ->  x = B, y = C, z = A (деление нацело, B != 0).
   Перебор с возвратом. Младшие m цифр произведения зависят только от
   младших m цифр множителей, поэтому буквы назначаются по столбцам справа
   налево, и как только столбец назначен целиком, проверяется
   (x mod 10^m) * (y mod 10^m) == z (mod 10^m). */

#include "rebus_internal.h"

/* (10^9)^2 помещается в long long, поэтому столбцы проверяем до 9-го */
#define MAX_CHECK_COLS 9

typedef struct search_t
{
    const word *x, *y, *z;
    int n_letters;
    /* k-й назначается буква с номером letter[k] */
    int letter[MAX_LETTERS];
    int min_digit[MAX_LETTERS];
    /* cols_known[k] - сколько младших столбцов полностью назначено,
       когда назначены первые k букв */
    int cols_known[MAX_LETTERS + 1];
    /* digits[i] - цифра буквы с номером i (как в rebus_t) */
    int digits[MAX_LETTERS];
    /* бит d установлен, если цифра d уже занята */
    unsigned used;
} search_t;

/* Число из младших m цифр слова (всё слово, если оно короче) */
static long long low_value(const word *w, const int digits[MAX_LETTERS], int m)
{
    long long value = 0;
    for (int i = w->len > m ? w->len - m : 0; i < w->len; i++)
        value = value * 10 + digits[w->letters[i]];
    return value;
}

/* Совпадают ли младшие m цифр x * y и z */
static bool low_digits_match(const search_t *s, int m)
{
    long long mod = 1;
    for (int i = 0; i < m; i++)
        mod *= 10;

    long long x = low_value(s->x, s->digits, m);
    long long y = low_value(s->y, s->digits, m);
    long long z = low_value(s->z, s->digits, m);
    return x * y % mod == z % mod;
}

/* Точная проверка x * y == z без переполнения: через деление */
static bool product_matches(const search_t *s)
{
    long long x = low_value(s->x, s->digits, MAX_WORD_LEN);
    long long y = low_value(s->y, s->digits, MAX_WORD_LEN);
    long long z = low_value(s->z, s->digits, MAX_WORD_LEN);

    if (x == 0)
        return z == 0;
    return z % x == 0 && z / x == y;
}

/* Назначает цифру k-й по порядку букве и уходит к следующей */
static bool search(search_t *s, int k)
{
    /* только что закрыли очередной столбец: проверяем младшие цифры */
    if (k > 0 && s->cols_known[k] > s->cols_known[k - 1] &&
        !low_digits_match(s, s->cols_known[k]))
        return false;

    if (k == s->n_letters)
        return product_matches(s);

    for (int d = s->min_digit[k]; d < 10; d++)
    {
        unsigned bit = 1u << d;
        if (s->used & bit)
            continue;

        s->used |= bit;
        s->digits[s->letter[k]] = d;
        if (search(s, k + 1))
            return true;
        s->used &= ~bit;
    }
    return false;
}

/* Ставит букву в конец порядка назначения, если её там ещё нет */
static void add_letter(search_t *s, bool in_order[MAX_LETTERS], const word *w, int col)
{
    if (col >= w->len)
        return;

    int letter = w->letters[w->len - 1 - col];
    if (!in_order[letter])
    {
        in_order[letter] = true;
        s->letter[s->n_letters++] = letter;
    }
}

bool solve_general(const rebus_t *rebus, int digits[MAX_LETTERS])
{
    search_t s = {0};
    const word *a = &rebus->operands[0];
    const word *b = &rebus->operands[1];
    bool is_div = rebus->ops[0] == DIV;

    s.x = is_div ? b : a;
    s.y = is_div ? &rebus->result : b;
    s.z = is_div ? a : &rebus->result;

    /* порядок назначения: по столбцам справа налево */
    bool in_order[MAX_LETTERS] = {false};
    for (int col = 0; col < MAX_WORD_LEN; col++)
    {
        add_letter(&s, in_order, s.x, col);
        add_letter(&s, in_order, s.y, col);
        add_letter(&s, in_order, s.z, col);

        if (col < MAX_CHECK_COLS)
            s.cols_known[s.n_letters] = col + 1;
    }
    for (int k = 1; k <= s.n_letters; k++)
        if (s.cols_known[k] < s.cols_known[k - 1])
            s.cols_known[k] = s.cols_known[k - 1];

    for (int k = 0; k < s.n_letters; k++)
        s.min_digit[k] = rebus->is_leading[s.letter[k]] ? 1 : 0;

    /* делитель из одной буквы не ведущий, но нулём быть не может */
    if (is_div && b->len == 1)
        for (int k = 0; k < s.n_letters; k++)
            if (s.letter[k] == b->letters[0])
                s.min_digit[k] = 1;

    if (!search(&s, 0))
        return false;

    for (int i = 0; i < MAX_LETTERS; i++)
        digits[i] = s.digits[i];
    return true;
}
