/**
 * @file   atmController.cpp
 * @brief  Implementation of the ATM Controller class.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <atm/atmController.h>
#include <atm/iBankService.h>
#include <atm/iCashBin.h>
#include <atm/money.h>

namespace atm {

    // Implementation of atmController methods 

        /**
         * @brief Check if the AtM status is idle to read the card number and change the status to awaiting PIN.
         * @param  cardNumber: The user's card number.
         * @return Status of the ATM SessionState.
         */
        SessionState atmController::insertCard(const std::string& cardNumber) {
            

            if (sessionState != SessionState::IDLE) {
                std::cerr << "Error: ATM is not idle. Cannot insert card." << std::endl;
                return sessionState;  // Return the current state without changing it
            }
            
            this->userCardNumber = cardNumber;
            this->sessionState = SessionState::AWAITING_PIN;
            return this->sessionState;
        }

        /**
         * @brief  Verify the PIN for a user's card.
         * @param  pin: The user's PIN.
         * @return ErrorState indicating the result of the PIN verification.
         */
        ErrorState atmController::enterPin(const std::string& pin) {
            if (sessionState != SessionState::AWAITING_PIN) {
                std::cerr << "Error: ATM is not awaiting PIN. Cannot enter PIN." << std::endl;
                return ErrorState::UNKNOWN_ERROR;  // Return an error state
            }
            // call the bank service to verify the PIN
            bool isPinValid = bankService->verifyPin(this->userCardNumber, pin);
            if (isPinValid) {
                // If the PIN is valid, retrieve the user's accounts
                userAccounts = bankService->getAccounts(this->userCardNumber);
                if (userAccounts.empty()) {
                    std::cerr << "Error: No accounts found for the user." << std::endl;
                    return ErrorState::INVALID_ACCOUNT;  // Return an error state
                }
                this->sessionState = SessionState::AWAITING_ACCOUNT_SELECTION;
                return ErrorState::OK;
            } else {
                return ErrorState::INVALID_PIN;
            }

        }


        /**
         * @brief Get the account selection for the user after the PIN is verified.
         * @param accountID: The user's selected account ID.
         * @return SessionState indicating the result of the account selection.
         */
        SessionState atmController::selectAccount(const std::string& accountID) {
            if (sessionState != SessionState::AWAITING_ACCOUNT_SELECTION) {
                std::cerr << "Error: ATM is not awaiting account selection. Cannot select account." << std::endl;
                return sessionState;  // Return the current state without changing it
            }
            // Check if the selected accountID is in the user's accounts
            auto it = std::find(userAccounts.begin(), userAccounts.end(), accountID);
            if (it != userAccounts.end()) {
                this->userAccountID = accountID;
                this->sessionState = SessionState::READY_FOR_TRANSACTION;
                return this->sessionState;
            } else {
                std::cerr << "Error: Invalid account ID selected." << std::endl;
                return SessionState::AWAITING_ACCOUNT_SELECTION;  // Remain in the same state
            }
        }


        /**
         * @brief  update and show the balance of a user's account.
         * @param  balance: The user's account balance.
         * @return The ErrorState indicating the result of the balance retrieval.
         */
        ErrorState atmController::showBalance(Money& balance) {
            if (sessionState != SessionState::READY_FOR_TRANSACTION) {
                std::cerr << "Error: ATM is not ready for transaction. Cannot show balance." << std::endl;
                return ErrorState::UNABLE_TO_SHOW_BALANCE;
            }
            // Call the bank service to get the account balance
            balance = bankService->getBalance(this->userAccountID);
            return ErrorState::OK;
        }

        /**
         * @brief  Deposit money into a user's account.
         * @param  amount: The amount of money to deposit.
         * @return The ErrorState indicating the result of the deposit operation.
         */
        ErrorState atmController::deposit(Money amount) {
            if (sessionState != SessionState::READY_FOR_TRANSACTION) {
                std::cerr << "Error: ATM is not ready for transaction. Cannot deposit money." << std::endl;
                return ErrorState::UNKNOWN_ERROR;
            }
            // check if the amount is valid (greater than 0)
            if (amount <= 0) {
                std::cerr << "Error: Invalid deposit amount." << std::endl;
                return ErrorState::INVALID_AMOUNT;
            }

            // deposit cash into the cash bin
            bool cashDepositSuccess = cashBin->deposit(amount);
            if (!cashDepositSuccess) {
                std::cerr << "Error: Failed to deposit cash." << std::endl;
                return ErrorState::UNABLE_TO_DEPOSIT_CASH_BIN;
            }
           
            // if the cash deposit is successful, call the bank service to deposit into the user's account
            bool accountDepositSuccess = bankService->deposit(this->userAccountID, amount);
            if (!accountDepositSuccess) {
                std::cerr << "Error: Failed to deposit into user's account." << std::endl;
                return ErrorState::FAILED_DEPOSIT_BANK_SERVICE;
            }

            return ErrorState::OK;
        }

        /**
         * @brief  Withdraw money from a user's account.
         * @param  amount: The amount of money to withdraw.
         * @return The ErrorState indicating the result of the withdrawal operation.
         */
        ErrorState atmController::withdraw(Money amount) {
            if (sessionState != SessionState::READY_FOR_TRANSACTION) {
                std::cerr << "Error: ATM is not ready for transaction. Cannot withdraw money." << std::endl;
                return ErrorState::UNKNOWN_ERROR;
            }
            // check if the amount is valid (greater than 0)
            if (amount <= 0) {
                std::cerr << "Error: Invalid withdrawal amount." << std::endl;
                return ErrorState::INVALID_AMOUNT;
            }

            // check if the cash bin has enough cash for the withdrawal
            Money cashBinBalance = cashBin->getBalance();
            if (cashBinBalance < amount) {
                std::cerr << "Error: Insufficient cash in the ATM for withdrawal." << std::endl;
                return ErrorState::INSUFFICIENT_CASH_BIN;
            }

            // call the bank service to withdraw money
            bool accountWithdrawalSuccess = bankService->withdraw(this->userAccountID, amount);
            if (!accountWithdrawalSuccess) {
                std::cerr << "Error: Insufficient funds in the user's account for withdrawal." << std::endl;
                return ErrorState::INSUFFICIENT_FUNDS;
            }

            // if the account withdrawal is successful, withdraw cash from the cash bin
            bool cashWithdrawalSuccess = cashBin->withdraw(amount);
            if (!cashWithdrawalSuccess) {
                std::cerr << "Error: Failed to withdraw cash from the ATM." << std::endl;
                /* If cash withdrawal fails, we should ideally roll back the account withdrawal, but for simplicity, we just return an error here. */
                return ErrorState::UNABLE_TO_WITHDRAW_CASH_BIN;
            }
        

            return ErrorState::OK;
        }

        /**
         * @brief  Eject the user's card and reset the session state.
         * @return SessionState indicating that the ATM is now idle.
         */
        SessionState atmController::ejectCard() {
            // Reset the session state and clear user data
            this->sessionState = SessionState::IDLE;
            this->errorState = ErrorState::OK;
            this->userCardNumber.clear();
            this->userAccountID.clear();
            this->userAccounts.clear();
            return this->sessionState;
        }



} // namespace atm





