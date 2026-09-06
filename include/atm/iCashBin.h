/**
 * @file   iCashBin.h
 * @brief  Interface for cash bin functionality.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */


#pragma once
#include <atm/money.h>

namespace atm {
    class iCashBin {
    public:

        // Destructor
        virtual ~iCashBin() = default;

        /**
         * @brief  Deposit money into the cash bin.
         * @param  amount: The amount of money to deposit.
         * @return True if the deposit was successful, false otherwise.
         */
        virtual bool deposit(Money amount) = 0;

        /**
         * @brief  Withdraw money from the cash bin.
         * @param  amount: The amount of money to withdraw.
         * @return True if the withdrawal was successful, false otherwise.
         */
        virtual bool withdraw(Money amount) = 0;   
        
        /**
         * @brief  Get the current balance of the cash bin.
         * @return The current balance as a Money type.
         */
        virtual Money getBalance() const = 0;

    };
}