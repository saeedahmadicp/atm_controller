/**
 * @file   fackData.h
 * @brief  Fake iBankService / iCashBin implementations used by the tests.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include <atm/iBankService.h>
#include <atm/iCashBin.h>
#include <atm/money.h>

namespace atm {
namespace test {

    class FakeBankService : public iBankService {

        private:
            std::map<std::string, std::string> pins_;
            std::map<std::string, std::vector<std::string>> cardAccounts_;
            std::map<std::string, Money> balances_;

        public:

            /**
             * @brief  Register a card with the bank, along with the accounts it can access.
             * @param  cardNumber: The card number to register.
             * @param  pin: The correct PIN for this card.
             * @param  accounts: The account IDs that belong to this card.
             */
            void addCard(const std::string& cardNumber, const std::string& pin,
                        const std::vector<std::string>& accounts) {
                this->pins_[cardNumber] = pin;
                this->cardAccounts_[cardNumber] = accounts;
            }

            /**
             * @brief  Set the starting balance of an account.
             * @param  accountID: The account to set the balance for.
             * @param  balance: The starting balance.
             */
            void setBalance(const std::string& accountID, Money balance) {
                this->balances_[accountID] = balance;
            }

            bool verifyPin(const std::string& cardNumber, const std::string& pin) override {
                auto it = this->pins_.find(cardNumber);
                return it != this->pins_.end() && it->second == pin;
            }

            std::vector<std::string> getAccounts(const std::string& cardNumber) override {
                auto it = this->cardAccounts_.find(cardNumber);
                return it != this->cardAccounts_.end() ? it->second : std::vector<std::string>{};
            }

            Money getBalance(const std::string& accountID) override {
                return this->balances_[accountID];
            }

            bool deposit(const std::string& accountID, Money amount) override {
                this->balances_[accountID] += amount;
                return true;
            }

            bool withdraw(const std::string& accountID, Money amount) override {
                if (this->balances_[accountID] < amount) {
                    return false;
                }
                this->balances_[accountID] -= amount;
                return true;
            }

    
    };

    class FakeCashBin : public iCashBin {
        private:
            Money cash_;
        public:
            explicit FakeCashBin(Money initialCash) : cash_(initialCash) {}

            bool deposit(Money amount) override {
                this->cash_ += amount;
                return true;
            }

            bool withdraw(Money amount) override {
                if (this->cash_ < amount) {
                    return false;
                }
                this->cash_ -= amount;
                return true;
            }

            Money getBalance() const override {
                return this->cash_;
            }

    
    };

} // namespace test
} // namespace atm
