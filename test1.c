#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void klee_make_symbolic(void *addr, size_t nbytes, const char *name);
uint64_t klee_uint64(const char *name) {
    uint64_t v;
    klee_make_symbolic(&v, sizeof(v), name);
    return v;
}

struct Account {
    uint64_t address;
    uint64_t balance;
};

int main() {
    struct Account a;
    a.address = klee_uint64("address");
    a.balance = klee_uint64("balance");

}