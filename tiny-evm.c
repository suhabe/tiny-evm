#include <stddef.h>
#include <cxxabi.h>
#include <string.h>
#include <assert.h>

#define MAX_NUM_ACCOUNTS 10

void klee_make_symbolic(void *addr, size_t nbytes, const char *name);

struct account {
    uint64_t address;
    uint64_t balance;
    uint64_t nonce;
};

struct state {
    uint64_t numAccounts;
    struct account accounts[MAX_NUM_ACCOUNTS];
};

struct transaction {
    uint64_t origin;
    uint64_t sender;
    uint64_t recipient;
    uint64_t value;
    uint64_t availableGas;
    uint64_t gasPrice;
};

uint64_t add_account(struct state *s, const uint64_t address) {
    for (uint64_t i = 0; i < s->numAccounts; i++) {
        if (s->accounts[i].address == address) {
            return i;
        }
    }

    assert(s->numAccounts + 1 < MAX_NUM_ACCOUNTS);
    s->numAccounts++;

    uint64_t i = s->numAccounts - 1;
    s->accounts[i].address = address;
    s->accounts[i].nonce = 0;
    s->accounts[i].balance = 0;
    return i;
}

struct account new_symbolic_account() {
    struct account a;
    memset(&a, 0, sizeof(struct account));
    klee_make_symbolic(&a.address, sizeof(a.address), "address");
    klee_make_symbolic(&a.balance, sizeof(a.balance), "balance");
    klee_make_symbolic(&a.nonce, sizeof(a.nonce), "nonce");
    return a;
}

//numAccounts must be less than MAX_NUM_ACCOUNTS
struct state new_symbolic_state(uint64_t numAccounts) {
    assert (numAccounts < MAX_NUM_ACCOUNTS);

    struct state s;
    memset(&s, 0, sizeof(struct state));
    s.numAccounts = numAccounts;
    for (uint64_t i = 0; i < sizeof(s.numAccounts); i++) {
        s.accounts[i] = new_symbolic_account();
    }
    return s;
}

struct transaction new_symbolic_transaction() {
    struct transaction t;
    memset(&t, 0, sizeof(struct state));
    klee_make_symbolic(&t.origin, sizeof(t.origin), "origin");
    klee_make_symbolic(&t.sender, sizeof(t.sender), "sender");
    klee_make_symbolic(&t.recipient, sizeof(t.recipient), "recipient");
    klee_make_symbolic(&t.value, sizeof(t.value), "value");
    klee_make_symbolic(&t.availableGas, sizeof(t.availableGas), "availableGas");
    klee_make_symbolic(&t.gasPrice, sizeof(t.gasPrice), "gasPrice");
    return t;
}

struct state apply(const struct state state0, const struct transaction tx) {
    struct state state1 = state0;

    uint64_t recipient_i = add_account(&state1, tx.recipient);
    uint64_t sender_i = add_account(&state1, tx.sender);

    if (tx.sender != tx.recipient) {
        state1.accounts[recipient_i].balance = state0.accounts[recipient_i].balance + tx.value;
        state1.accounts[sender_i].balance = state0.accounts[sender_i].balance + tx.value;
    }

    return state1;
}

int main() {
    const uint64_t numAccounts = 2;

    struct state state_init = new_symbolic_state(numAccounts);
    struct transaction tx = new_symbolic_transaction();
    struct state state_final = apply(state_init, tx);
}