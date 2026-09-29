#ifndef REBUS_INTERNAL_H
#define REBUS_INTERNAL_H

#include <stdbool.h>

#include "rebus.h"

#define MAX_OPERANDS 7
#define MAX_LETTERS 10
#define MAX_WORD_LEN 15

typedef enum operations
{
    ADD,
    SUB,
    MUL,
    DIV
} operations;

typedef struct word
{
    /* Индексы букв. (Парсер идёт слева направо и нумерует новые буквы) */
    int letters[MAX_WORD_LEN];
    int len;
    int sign;
} word;

typedef struct rebus_t
{
    word operands[MAX_OPERANDS];
    int n_operands;
    operations ops[MAX_OPERANDS - 1];
    word result;

    int n_letters;
    /* индекс буквы - символ ('A'..'Z') */
    char letter_char[MAX_LETTERS];
    /* по символу узнать индекс буквы в ребусе
    символ  - 'A' = индекс буквы в ребусе, -1 если буквы нет в ребусе */
    int letter_index[26];
    /* Является ли буква ведущей */
    bool is_leading[MAX_LETTERS];

    /* есть * или / => решаем через solve_general */
    bool has_mul_div;
} rebus_t;

/* Разбирает строку в rebus */
int rebus_parse(const char *input, rebus_t *rebus);

/* Решатели ищут цифру для каждой буквы. Возвращают true, если решение найдено. */
bool solve_linear(const rebus_t *rebus, int digits[MAX_LETTERS]);  /* + и - */
bool solve_general(const rebus_t *rebus, int digits[MAX_LETTERS]); /* * и / */

/* Форматирование ответа */
int rebus_format(const rebus_t *rebus, const char *input,
                 const int digits[MAX_LETTERS], char *out, int size);

#endif
