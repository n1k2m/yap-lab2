#include <stdio.h>

#include "rebus.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define OUT_SIZE 256
#define MIN_TIME 0.5 /* секунд на один ребус */

static const char *puzzles[] = {
    "BE + BE = MOO",
    "SEND + MORE = MONEY",
    "BIG + CAT = LION",
    "CROSS + ROADS = DANGER",
    "TEN + TEN + FORTY = SIXTY",
    "ELEVEN + NINE + FIVE + FIVE = THIRTY",
};

/* Текущее время в секундах */
static double now(void)
{
#ifdef _WIN32
    static LARGE_INTEGER freq;
    LARGE_INTEGER t;
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)freq.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
#endif
}

/* Среднее время одного решения в миллисекундах, -1 при ошибке */
static double measure(const char *puzzle)
{
    char out[OUT_SIZE];
    int runs = 0;
    double start = now();
    double elapsed;

    do
    {
        if (rebus_solve(puzzle, out, OUT_SIZE) != REBUS_OK)
            return -1;
        runs++;
        elapsed = now() - start;
    } while (elapsed < MIN_TIME);

    return elapsed / runs * 1000.0;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    /* исходник в UTF-8: просим консоль Windows показывать вывод как UTF-8 */
    SetConsoleOutputCP(CP_UTF8);
#endif

    const char *label = argc > 1 ? argv[1] : "время";
    int n_puzzles = (int)(sizeof(puzzles) / sizeof(puzzles[0]));

    printf("| Ребус | %s, мс |\n", label);
    printf("|---|---:|\n");

    for (int i = 0; i < n_puzzles; i++)
    {
        printf("| %s |", puzzles[i]);
        fflush(stdout);

        double ms = measure(puzzles[i]);
        if (ms < 0)
            printf(" error |\n");
        else
            printf(" %.4f |\n", ms);
    }
    return 0;
}
