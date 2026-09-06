/**
 * @file   atmControllerTest.cpp
 * @brief  Tests for the atmController class, using FakeBankService/FakeCashBin.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#include <iostream>

#include <atm/atmController.h>

#include "fackData.h"
#include "testRunner.h"

using namespace atm;
using namespace atm::test;

// insert -> PIN -> select -> balance -> deposit -> withdraw -> eject
void test_fullFlow() {
    FakeBankService bank;
    bank.addCard("1111222233334444", "1234", {"acc-1"});
    bank.setBalance("acc-1", 100);
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    CHECK_EQ(atm.insertCard("1111222233334444"), SessionState::AWAITING_PIN);
    CHECK_EQ(atm.enterPin("1234"), ErrorState::OK);
    CHECK_EQ(atm.selectAccount("acc-1"), SessionState::READY_FOR_TRANSACTION);

    Money balance = 0;
    CHECK_EQ(atm.showBalance(balance), ErrorState::OK);
    CHECK_EQ(balance, 100);

    CHECK_EQ(atm.deposit(50), ErrorState::OK);
    atm.showBalance(balance);
    CHECK_EQ(balance, 150);

    CHECK_EQ(atm.withdraw(70), ErrorState::OK);
    atm.showBalance(balance);
    CHECK_EQ(balance, 80);

    CHECK_EQ(atm.ejectCard(), SessionState::IDLE);
}

// A wrong PIN fails without ejecting the card; the correct PIN then succeeds.
void test_wrongPinThenRetry() {
    FakeBankService bank;
    bank.addCard("1111", "1234", {"acc-1"});
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    atm.insertCard("1111");
    CHECK_EQ(atm.enterPin("0000"), ErrorState::INVALID_PIN);
    CHECK_EQ(atm.enterPin("1234"), ErrorState::OK);
}

// Selecting an account that isn't on the card leaves the session state unchanged.
void test_unknownAccount() {
    FakeBankService bank;
    bank.addCard("1111", "1234", {"acc-1"});
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    atm.insertCard("1111");
    atm.enterPin("1234");
    CHECK_EQ(atm.selectAccount("acc-unknown"), SessionState::AWAITING_ACCOUNT_SELECTION);
}

// Calling methods out of order (no card / no PIN / no account) should fail cleanly.
void test_stateGuards() {
    FakeBankService bank;
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    Money balance = 0;
    CHECK_EQ(atm.enterPin("1234"), ErrorState::UNKNOWN_ERROR);
    CHECK_EQ(atm.showBalance(balance), ErrorState::UNABLE_TO_SHOW_BALANCE);
    CHECK_EQ(atm.deposit(10), ErrorState::UNKNOWN_ERROR);
    CHECK_EQ(atm.withdraw(10), ErrorState::UNKNOWN_ERROR);
}

// Withdrawing more than the account balance.
void test_withdrawInsufficientFunds() {
    FakeBankService bank;
    bank.addCard("1111", "1234", {"acc-1"});
    bank.setBalance("acc-1", 20);
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    atm.insertCard("1111");
    atm.enterPin("1234");
    atm.selectAccount("acc-1");

    CHECK_EQ(atm.withdraw(50), ErrorState::INSUFFICIENT_FUNDS);
}

// Withdrawing more than the ATM's cash bin holds.
void test_withdrawInsufficientCash() {
    FakeBankService bank;
    bank.addCard("1111", "1234", {"acc-1"});
    bank.setBalance("acc-1", 500);
    FakeCashBin cashBin(30);
    atmController atm(&bank, &cashBin);

    atm.insertCard("1111");
    atm.enterPin("1234");
    atm.selectAccount("acc-1");

    CHECK_EQ(atm.withdraw(50), ErrorState::INSUFFICIENT_CASH_BIN);
}

// Negative deposit and zero withdrawal are both rejected.
void test_invalidAmount() {
    FakeBankService bank;
    bank.addCard("1111", "1234", {"acc-1"});
    bank.setBalance("acc-1", 100);
    FakeCashBin cashBin(1000);
    atmController atm(&bank, &cashBin);

    atm.insertCard("1111");
    atm.enterPin("1234");
    atm.selectAccount("acc-1");

    CHECK_EQ(atm.deposit(-10), ErrorState::INVALID_AMOUNT);
    CHECK_EQ(atm.withdraw(0), ErrorState::INVALID_AMOUNT);
}

int main() {
    RUN(test_fullFlow);
    RUN(test_wrongPinThenRetry);
    RUN(test_unknownAccount);
    RUN(test_stateGuards);
    RUN(test_withdrawInsufficientFunds);
    RUN(test_withdrawInsufficientCash);
    RUN(test_invalidAmount);

    if (atm::test::failureCount == 0) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }
    std::cout << atm::test::failureCount << " check(s) failed." << std::endl;
    return 1;
}
