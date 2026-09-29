#ifndef REBUS_H
#define REBUS_H

/* Коды возврата */
#define REBUS_OK 0              // решение найдено, ответ записан в out
#define REBUS_ERR_PARSE 1       // некорректный ввод
#define REBUS_ERR_NO_SOLUTION 2 // ввод корректный, но решения нет
#define REBUS_ERR_BUFFER 3      // out == NULL или ответ не помещается в size байт

/* Решает ребус input */
int rebus_solve(const char *input, char *out, int size);

/* Текстовое описание кода возврата */
const char *rebus_strerror(int code);

#endif
