#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define MAX_NUM_ACCOUNTS 10
void klee_assume(int c)  {}

struct Account {
    uint64_t address;
    uint64_t balance;
    uint64_t nonce;
};

struct State {
    uint64_t numAccounts;
    struct Account accounts[MAX_NUM_ACCOUNTS];
};

struct Transaction {
    uint64_t origin;
    uint64_t sender;
    uint64_t recipient;
    uint64_t value;
    uint64_t availableGas;
    uint64_t gasPrice;
};

uint64_t add_account(struct State *s, const uint64_t address) {
    for (uint64_t i = 0; i < s->numAccounts; i++) {
        if (s->accounts[i].address == address) {
            return i;
        }
    }

    assert(s->numAccounts + 1 < MAX_NUM_ACCOUNTS);
    s->numAccounts++;
    printf("%s\n", "tx.recipient new account");
    uint64_t i = s->numAccounts - 1;
    s->accounts[i].address = i;
    s->accounts[i].nonce = 0;
    s->accounts[i].balance = 0;
    return i;
}

struct State apply(const struct State state0, const struct Transaction tx) {
    struct State state1 = state0;

    uint64_t recipient_i = add_account(&state1, tx.recipient);
    uint64_t sender_i = add_account(&state1, tx.sender);

    if (tx.sender != tx.recipient) {
        printf("%s\n", "tx.sender != tx.recipient");
        state1.accounts[recipient_i].balance = state0.accounts[recipient_i].balance + tx.value;
        state1.accounts[sender_i].balance = state0.accounts[sender_i].balance + tx.value;
    }

    return state1;
}


///////////////////////////////////////////////////////////////////////////////

void klee_make_symbolic(void *addr, size_t nbytes, const char *name)  {}

uint64_t klee_symbolic_uint64(const char *name) {
    uint64_t v;
    klee_make_symbolic(&v, sizeof(v), name);
    return v;
}

struct Account new_symbolic_account(uint64_t index) {
    struct Account a;
    memset(&a, 0, sizeof(struct Account));
    a.address = index;
    a.balance = klee_symbolic_uint64("balance");
    a.nonce = klee_symbolic_uint64("nonce");
    return a;
}

//numAccounts must be less than MAX_NUM_ACCOUNTS
struct State new_symbolic_state(uint64_t numAccounts) {
    assert (numAccounts < MAX_NUM_ACCOUNTS);

    struct State s;
    memset(&s, 0, sizeof(struct State));
    s.numAccounts = numAccounts;
    for (uint64_t i = 0; i < s.numAccounts; i++) {
        s.accounts[i] = new_symbolic_account(i);
    }
    return s;
}

struct Transaction new_symbolic_transaction() {
    struct Transaction t;
    memset(&t, 0, sizeof(struct Transaction));
    t.origin = klee_symbolic_uint64("address");
    t.sender = klee_symbolic_uint64("sender");
    t.recipient = klee_symbolic_uint64("recipient");
    t.value = klee_symbolic_uint64("value");
    t.availableGas = klee_symbolic_uint64("availableGas");
    t.gasPrice = klee_symbolic_uint64("gasPrice");
    return t;
}
///////////////////////////////////////////////////////////////////////////////

struct Transaction new_concrete_transaction() {
    struct Transaction t;
    memset(&t, 0, sizeof(struct Transaction));
    return t;
}

struct Account new_concrete_account(uint64_t index) {
    struct Account a;
    memset(&a, 0, sizeof(struct Account));
    a.address = index;
    a.balance = 0;
    a.nonce = 0;
    return a;
}

struct State new_concrete_state(uint64_t numAccounts) {
    assert (numAccounts < MAX_NUM_ACCOUNTS);

    struct State s;
    memset(&s, 0, sizeof(struct State));
    s.numAccounts = numAccounts;
    for (uint64_t i = 0; i < s.numAccounts; i++) {
        s.accounts[i] = new_concrete_account(i);
    }
    return s;
}


////////////////////////////////////////////////////////////////////////////////
int main() {
    const uint64_t numAccounts = 2;

    if (1) {
        struct State state_init = new_symbolic_state(numAccounts);
        struct Transaction tx = new_symbolic_transaction();
        struct State state_final = apply(state_init, tx);
    } else {
        struct State state_init = new_concrete_state(numAccounts);
        struct Transaction tx = new_concrete_transaction();
        tx.recipient = 5;
        tx.value = 10;
        struct State state_final = apply(state_init, tx);
    }
}