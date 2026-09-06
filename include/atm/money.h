/**
 * @file   money.h
 * @brief  Money class for handling monetary values across different classes.
 * @author Saeed Ahmad
 * @date   September 6, 2026
 */


#pragma once 

namespace atm {
    using Money = long long;  // As the smallest unit is $1, so we can use int (long long: 64 bits) to represent money in dollars.
} 