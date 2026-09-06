/**
 * @file   testRunner.h
 * @brief  Minimal test harness used by the ATM controller tests.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */

#pragma once

#include <iostream>
#include <string>

namespace atm {
namespace test {

    // Total number of failed checks across all tests.
    inline int failureCount = 0;

    /**
     * @brief  Record a check failure with its location and message.
     * @param  file: Source file where the check ran.
     * @param  line: Source line where the check ran.
     * @param  message: What was being checked.
     */
    inline void reportFailure(const char* file, int line, const std::string& message) {
        std::cerr << "FAIL: " << file << ":" << line << " - " << message << std::endl;
        ++failureCount;
    }

} // namespace test
} // namespace atm

// Fails the test if condition is false.
#define CHECK(condition) \
    if (!(condition)) atm::test::reportFailure(__FILE__, __LINE__, #condition)

// Fails the test if actual and expected are not equal.
#define CHECK_EQ(actual, expected) \
    if (!((actual) == (expected))) atm::test::reportFailure(__FILE__, __LINE__, #actual " == " #expected)

// Runs a test function and prints its name.
#define RUN(testFunc) \
    std::cout << "Running " #testFunc "..." << std::endl; \
    testFunc()
