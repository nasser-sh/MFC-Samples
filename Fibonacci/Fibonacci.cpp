/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include <vector>
#include "Fibonacci.h"


namespace
{
    int Fibonacci(int n, std::vector<int> &fibCache)
    {
        if (fibCache[n] == 0) {
            fibCache[n] = Fibonacci(n - 1, fibCache) + Fibonacci(n - 2, fibCache);
        }

        return fibCache[n];
    }
}


namespace fibonacci
{
    int Fibonacci(int n)
    {
        if (n <= 1) {
            return 1;
        }

        std::vector<int> fibCache(n, 0);
        fibCache[0] = 1;
        fibCache[1] = 1;

        return ::Fibonacci(n - 1, fibCache);
    }
}