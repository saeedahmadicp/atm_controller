/**
 * @file   IBankService.h
 * @brief  Interface for bank service functionality.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#pragma once
#include <string>
#include <vector>

#include <atm/money.h>

using namespace std;

namespace atm {
   
    class iBankService {
    public:
        // Destructor
        virtual ~iBankService() = default;

        /**
         * @brief  Verify the PIN for a user's card.
         * @param  cardNumber: The user's card number.
         * @param  pin: The user's PIN.
         * @return True if the PIN is valid, false otherwise.
         */
        virtual bool verifyPin(const string& cardNumber, const string& pin) = 0;
        

        /**
         * @brief  Get the accounts associated with a user's card.
         * @param  cardNumber: The user's card number.
         * @return A list of the user's account numbers if the PIN is valid, an empty list otherwise.
         */
        virtual std::vector<string> getAccounts(const string& cardNumber) = 0;


        /**
         * @brief  Get the balance of a user's account.
         * @param  accountNumber: The user's account number.
         * @return The current balance as a Money type.
         */
        virtual Money getBalance(const string& accountNumber) = 0;

        /**
         * @brief  Deposit money into a user's account.
         * @param  accountNumber: The user's account number.
         * @param  amount: The amount of money to deposit.
         * @return True if the deposit was successful, false otherwise.
         */
        virtual bool deposit(const string& accountNumber, Money amount) = 0;

        /**
         * @brief  Withdraw money from a user's account.
         * @param  accountNumber: The user's account number.
         * @param  amount: The amount of money to withdraw.
         * @return True if the withdrawal was successful, false otherwise. (false = insufficient funds)
         */
        virtual bool withdraw(const string& accountNumber, Money amount) = 0;
    };
}