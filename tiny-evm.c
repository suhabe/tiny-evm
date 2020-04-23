#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_ACCOUNTS 10

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

uint64_t get_value(int is_sym, char** argv, int argi, char* name) {
    if (is_sym) {
        return klee_symbolic_uint64(name);
    } else {
        return atoi(argv[argi]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "%s", "usage: tiny-evm <numAccounts> ...");
        exit(1);
    }

    int is_sym = (argc == 2);

    int argi = 1;
    int numAccounts = atoi(argv[argi++]);
    assert (numAccounts < MAX_NUM_ACCOUNTS);

    struct State initial_state;
    memset(&initial_state, 0, sizeof(initial_state));
    initial_state.numAccounts = numAccounts;
    for (int i = 0; i < initial_state.numAccounts; i++) {
        struct Account account;
        account.address = i;
        account.balance = get_value(is_sym, argv, argi++, "balance");
        account.nonce = get_value(is_sym, argv, argi++, "nonce");
    }

    struct Transaction tx;
    memset(&tx, 0, sizeof(tx));
    tx.origin = get_value(is_sym, argv, argi++, "origin");
    tx.sender = get_value(is_sym, argv, argi++,  "sender");
    tx.recipient = get_value(is_sym, argv, argi++,  "recipient");
    tx.value = get_value(is_sym, argv, argi++, "value");
    tx.availableGas = get_value(is_sym, argv, argi++,  "availableGas");
    tx.gasPrice = get_value(is_sym, argv, argi++,  "gasPrice");

    struct State final_state = apply(initial_state, tx);
}