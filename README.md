# ATM Controller

A C++ ATM Controller that implements the basic ATM operations such as:
 Insert Card -> PIN number -> Select Account -> See Balance / Deposit /
 Withdraw. The current code also provides template header files for other controllers (`iBankService`, `iCashBin`), for extending the current code to a real bank or ATM hardware, and there is a fake version of their implementation in the tests directory for testing.


## Requirements

- A C++17-compatible compiler (e.g. AppleClang, GCC, Clang)
- CMake ≥ 3.16

## Installation
To install the current code, clone the current repository, i.e., 

```bash
git clone https://github.com/saeedahmadicp/atm_controller.git
```

Build the code:

```bash
cmake -S . -B build
cmake --build build
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

> **Note:** `ctest` reports pass/fail for the whole `atm_tests` binary only, not per test case; run `./build/atm_tests` to see each test individually. No framework (e.g. GoogleTest) is used, to keep the implementation dependency-free.

Test code: [tests/atmControllerTest.cpp](tests/atmControllerTest.cpp) (fakes in [tests/fackData.h](tests/fackData.h), harness in [tests/testRunner.h](tests/testRunner.h)).

## Specification
Flow: Insert Card → PIN → Select Account → See Balance / Deposit / Withdraw

The ATM only accepts whole-dollar amounts (no cents). Session state moves forward as each step succeeds; ejectCard returns the session to idle from any state.

## Integration Guidelines
To connect a real bank system or ATM hardware, implement these two interfaces and pass instances into atmController's constructor:

[`iBankService`](include/atm/iBankService.h) — verifies PINs, looks up accounts, and reads/writes account balances against the bank.

[`iCashBin`](include/atm/iCashBin.h) — deposits/withdraws physical cash and reports how much the machine currently holds.
