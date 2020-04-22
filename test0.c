#include <stddef.h>

void klee_make_symbolic(void *addr, size_t nbytes, const char *name) {}

int main()
{
    int a;
    klee_make_symbolic(&a, sizeof(a), "a");

    if (a == 0)
        return 0;

    if (a < 0)
        return -1;
    else
        return 1;
}