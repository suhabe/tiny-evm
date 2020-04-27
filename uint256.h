/*******************************************************************************
*   Ledger Ethereum App
*   (c) 2016-2019 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

// Adapted from https://github.com/calccrypto/uint256_t

#ifndef _UINT256_H_
#define _UINT256_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct uint128_t { uint64_t elements[2]; } uint128_t;
typedef struct uint256_t { uint128_t elements[2]; } uint256_t;

#define UPPER(x) x.elements[0]
#define LOWER(x) x.elements[1]

uint128_t zero128() {
    uint128_t target;
    UPPER(target) = 0;
    LOWER(target) = 0;
    return target;
}

uint256_t zero256() {
    uint256_t target;
    UPPER(target) = zero128();
    LOWER(target) = zero128();
    return target;
}

bool gt128(const uint128_t number1, uint128_t number2) {
    if (UPPER(number1) == UPPER(number2)) {
        return (LOWER(number1) > LOWER(number2));
    }
    return (UPPER(number1) > UPPER(number2));
}

bool equal128(const uint128_t number1, const uint128_t number2) {
    return (UPPER(number1) == UPPER(number2)) &&
           (LOWER(number1) == LOWER(number2));
}

bool equal256(uint256_t number1, const uint256_t number2) {
    return (equal128(UPPER(number1), UPPER(number2)) &&
            equal128(LOWER(number1), LOWER(number2)));
}

uint128_t add128(uint128_t number1, uint128_t number2) {
    uint128_t target;
    UPPER(target) =
            UPPER(number1) + UPPER(number2) +
            ((LOWER(number1) + LOWER(number2)) < LOWER(number1));
    LOWER(target) = LOWER(number1) + LOWER(number2);
    return target;
}

uint256_t add256(const uint256_t number1, const uint256_t number2) {
    uint256_t target;
    uint128_t tmp;
    UPPER(target) = add128(UPPER(number1), UPPER(number2));
    tmp = add128(LOWER(number1), LOWER(number2));
    if (gt128(LOWER(number1), tmp)) {
        uint128_t one;
        UPPER(one) = 0;
        LOWER(one) = 1;
        UPPER(target) = add128(UPPER(target), one);
    }
    LOWER(target) = add128(LOWER(number1), LOWER(number2));
    return target;
}

uint128_t minus128(const uint128_t number1, const uint128_t number2) {
    uint128_t target;
    UPPER(target) =
            UPPER(number1) - UPPER(number2) -
            ((LOWER(number1) - LOWER(number2)) > LOWER(number1));
    LOWER(target) = LOWER(number1) - LOWER(number2);
    return target;
}

uint256_t minus256(const uint256_t number1, const uint256_t number2) {
    uint256_t target;
    uint128_t tmp;
    UPPER(target) = minus128(UPPER(number1), UPPER(number2));
    tmp = minus128(LOWER(number1), LOWER(number2));
    if (gt128(tmp, LOWER(number1))) {
        uint128_t one;
        UPPER(one) = 0;
        LOWER(one) = 1;
        UPPER(target) = minus128(UPPER(target), one);
    }
    LOWER(target) = minus128(LOWER(number1), LOWER(number2));
    return target;
}

#endif /* _UINT256_H_ */