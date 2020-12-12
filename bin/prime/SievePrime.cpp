/*
 * Copyright (C) 2015 Niek Linnenbank
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <String.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include "SievePrime.h"

SievePrime::SievePrime(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Compute prime numbers using the Sieve of Eratosthenes algorithm");
    parser().registerPositional("NUMBER", "Maximum number to search for prime numbers");
    parser().registerFlag('o', "stdout", "Write results to standard output if set");
}

SievePrime::~SievePrime()
{
}

SievePrime::Result SievePrime::exec()
{
    uint *map;
    int n, k = 2, i, last, sqrt_of_n;

    // Read max number
    n = atoi(arguments().get("NUMBER"));
    sqrt_of_n = sqrt(n);

    // Try to allocate memory
    if ((map = (uint *) malloc(n * sizeof(uint))) == NULL)
    {
        ERROR("malloc failed: " << strerror(errno));
        return IOError;
    }

    // Set to true
    for (i = 0; i < n; i++)
        map[i] = 1;

    // Find all primes below sqrt(n) sequentially
    searchSequential(sqrt_of_n, map);

    // Now continue above sqrt(n) sequentially
    while (k < sqrt_of_n)
    {
        // Prime number?
        if (!map[k])
        {
            k++;
            continue;
        }

        // Mark multiples of k in my range
        i    = sqrt_of_n;
        last = n;

        while (i < last)
        {
            // Do we need to unmark this number? (no prime)
            if (!(i % k))
                map[i] = 0;

            i++;
        }
        // Look for the next prime
        k++;
    }

    reportResult(n, map);

    // Done
    return Success;
}

SievePrime::Result SievePrime::reportResult(const int n,
                                            const unsigned *map) const
{
    // Print the result
    if (arguments().get("stdout"))
    {
        String output;
        int written = 0;

        for (int i = 2; i < n; i++)
        {
            if (map[i] == 1)
            {
                output << " " << i;
                written++;
            }

            if (written >= 32 || i == n - 1)
            {
                output << "\r\n";
                write(1, *output, output.length());
                output = "";
                written = 0;
            }
        }
    }

    return Success;
}

SievePrime::Result SievePrime::searchSequential(int n, unsigned *map) const
{
    int i, j;

    // Sequential algorithm
    // Next is a prime
    for (i = 2; i < n; i++)
    {
        // Prime number?
        if (map[i])
        {
            // Mask off all multiples
            for (j = i + 1; j < n; j++)
            {
                if (!(j % i))
                    map[j] = 0;
            }
        }
    }

    return Success;
}
