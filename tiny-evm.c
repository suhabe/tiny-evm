#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint256.h"

#define MAX_NUM_ACCOUNTS 10

struct Account {
    uint256_t address;
    uint256_t balance;
    uint256_t nonce;
};

struct State {
    uint64_t numAccounts;
    struct Account accounts[MAX_NUM_ACCOUNTS];
};

struct Transaction {
    uint256_t origin;
    uint256_t sender;
    uint256_t recipient;
    uint256_t value;
    uint256_t availableGas;
    uint256_t gasPrice;
};

uint64_t add_account(struct State *s, const uint256_t address) {
    for (uint64_t i = 0; i < s->numAccounts; i++) {
        if (equal256(s->accounts[i].address,address)) {
            return i;
        }
    }

    assert(s->numAccounts + 1 < MAX_NUM_ACCOUNTS);
    s->numAccounts++;
    printf("%s\n", "tx.recipient new account");
    uint64_t i = s->numAccounts - 1;
    s->accounts[i].address = address;
    s->accounts[i].nonce = zero256();
    s->accounts[i].balance = zero256();
    return i;
}

struct State apply(const struct State state0, const struct Transaction tx) {
    struct State state1 = state0;

    uint64_t recipient_i = add_account(&state1, tx.recipient);
    uint64_t sender_i = add_account(&state1, tx.sender);

    if (!equal256(tx.sender,tx.recipient)) {
        printf("%s\n", "tx.sender != tx.recipient");
        state1.accounts[recipient_i].balance = add256(state0.accounts[recipient_i].balance, tx.value);
        state1.accounts[sender_i].balance = minus256(state0.accounts[sender_i].balance, tx.value);
    }

    return state1;
}


///////////////////////////////////////////////////////////////////////////////

void klee_make_symbolic(void *addr, size_t nbytes, const char *name)  {}
void klee_assume(bool c) {}

uint64_t klee_symbolic_uint64(const char *name) {
    uint64_t v;
    klee_make_symbolic(&v, sizeof(v), name);
    return v;
}

uint256_t get_value(int is_sym, char** argv, int argi, char* name) {
    uint256_t v;
    if (is_sym) {
        v.elements[0].elements[0] = klee_symbolic_uint64(name);
        v.elements[0].elements[1] = klee_symbolic_uint64(name);
        v.elements[1].elements[0] = klee_symbolic_uint64(name);
        v.elements[1].elements[1] = klee_symbolic_uint64(name);
    } else {
        //printf("%s0: %s %ull %llu\n", name, argv[argi], atoi(argv[argi]), strtoull(argv[argi], NULL, 0));
        //printf("%s1: %s %ull %llu\n", name, argv[argi+1], atoi(argv[argi+1]), strtoull(argv[argi+1], NULL, 0));
        //printf("%s2: %s %ull %llu\n", name, argv[argi+2], atoi(argv[argi+2]), strtoull(argv[argi+2], NULL, 0));
        //printf("%s3: %s %ull %llu\n", name, argv[argi+3], atoi(argv[argi+3]), strtoull(argv[argi+3], NULL, 0));

        v.elements[0].elements[0] = strtoull(argv[argi], NULL, 0);
        v.elements[0].elements[1] = strtoull(argv[argi+1], NULL, 0);
        v.elements[1].elements[0] = strtoull(argv[argi+2], NULL, 0);
        v.elements[1].elements[1] = strtoull(argv[argi+3], NULL, 0);
    }
    return v;
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
        initial_state.accounts[i].address = get_value(is_sym, argv, argi, "address"); argi+=4;
        //initial_state.accounts[i].address = zero256();
        //initial_state.accounts[i].address.elements[0].elements[0] = i;
        initial_state.accounts[i].balance = get_value(is_sym, argv, argi, "balance"); argi+=4;
        initial_state.accounts[i].nonce = get_value(is_sym, argv, argi, "nonce"); argi+=4;

        for (int j = 0; j < i; j++) {
          //  klee_assume(!equal256(&initial_state.accounts[i].address, &initial_state.accounts[j].address));
        }
    }

    struct Transaction tx;
    memset(&tx, 0, sizeof(tx));
    tx.origin = get_value(is_sym, argv, argi, "origin"); argi+=4;
    tx.sender = get_value(is_sym, argv, argi,  "sender"); argi+=4;
    tx.recipient = get_value(is_sym, argv, argi,  "recipient"); argi+=4;
    tx.value = get_value(is_sym, argv, argi, "value"); argi+=4;
    tx.availableGas = get_value(is_sym, argv, argi,  "availableGas"); argi+=4;
    tx.gasPrice = get_value(is_sym, argv, argi,  "gasPrice"); argi+=4;

    struct State final_state = apply(initial_state, tx);
}