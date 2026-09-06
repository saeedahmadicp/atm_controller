/**
 * @file  atmController.h
 * @brief  Header file for the ATM Controller class, which manages the overall operation of the ATM system.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#pragma once
#include <string>
#include <vector>

#include <atm/iBankService.h>
#include <atm/iCashBin.h>
#include <atm/money.h>

namespace atm {

// defining the session for the ATM controller
enum class SessionState {
    IDLE,  
    AWAITING_PIN,
    AWAITING_ACCOUNT_SELECTION,
    READY_FOR_TRANSACTION,
};

// defining other states for the ATM controller to manage the errors 
enum class ErrorState {
    OK,           // No error
    INVALID_PIN,    // Invalid PIN entered
    INSUFFICIENT_FUNDS, // Insufficient funds for withdrawal in the user's account
    INSUFFICIENT_CASH_BIN, // Insufficient cash in the ATM for withdrawal
    UNABLE_TO_SHOW_BALANCE, // Unable to retrieve balance from the bank service
    INVALID_AMOUNT, // Invalid amount entered for deposit or withdrawal
    ACCOUNT_LOCKED, // User's account is locked (e.g., due to too many failed PIN attempts)
    INVALID_ACCOUNT, // Invalid account number selected
    UNABLE_TO_WITHDRAW_CASH_BIN, // Unable to withdraw cash from the ATM's cash bin
    UNABLE_TO_DEPOSIT_CASH_BIN, // Unable to deposit cash into the ATM's cash bin
    FAILED_DEPOSIT_BANK_SERVICE, // Failed to deposit into the user's account via the bank service
    FAILED_WITHDRAWAL_BANK_SERVICE, // Failed to withdraw from the user's account via the bank service
    UNKNOWN_ERROR  // An unknown error occurred
};


class atmController { 
    private:
        iBankService* bankService;  // Pointer to the bank service interface
        iCashBin* cashBin;          // Pointer to the cash bin interface
        SessionState sessionState;  // Current session state
        ErrorState errorState;      // Current error state

        // personal data for the current session
        std::string userCardNumber;  // User's card number
        std::string userAccountID; // User's selected account ID
        std::vector<std::string> userAccounts; // List of user's account numbers

    public:
        // Constructor
        atmController(iBankService* bankService, iCashBin* cashBin, SessionState sessionState = SessionState::IDLE, ErrorState errorState = ErrorState::OK):
            bankService(bankService), cashBin(cashBin), sessionState(sessionState), errorState(errorState) {};

        // Destructor
        ~atmController() = default;

        /**
         * @brief  Check if the AtM status is idle to read the card number and change the status to awaiting PIN.
         * @param  cardNumber: The user's card number.
         * @return Status of the ATM SessionState.
         */
        SessionState insertCard(const std::string& cardNumber);

        /**
         * @brief  Verify the PIN for a user's card.
         * @param  pin: The user's PIN.
         * @return ErrorState indicating the result of the PIN verification.
         */
        ErrorState enterPin(const std::string& pin);


        /**
         * @brief Get the account selection for the user after the PIN is verified.
         * @param accountID: The user's selected account ID.
         * @return SessionState indicating the result of the account selection.
         */
        SessionState selectAccount(const std::string& accountID);

        /**
         * @brief  update and show the balance of a user's account.
         * @param  balance: The user's account balance.
         * @return The ErrorState indicating the result of the balance retrieval.
         */
        ErrorState showBalance(Money& balance);

        /**
         * @brief  Deposit money into a user's account.
         * @param  amount: The amount of money to deposit.
         * @return The ErrorState indicating the result of the deposit operation.
         */
        ErrorState deposit(Money amount);

        /**
         * @brief  Withdraw money from a user's account.
         * @param  amount: The amount of money to withdraw.
         * @return The ErrorState indicating the result of the withdrawal operation.
         */
        ErrorState withdraw(Money amount);

        /**
         * @brief  Eject the user's card and reset the session state.
         * @return SessionState indicating that the ATM is now idle.
         */
        SessionState ejectCard();

}; 

}