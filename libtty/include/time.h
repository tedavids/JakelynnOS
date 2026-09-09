// time functions

/* Copyright (C) 2026 Tom Davidson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

typedef struct {
    uint8_t     second;
    uint8_t     minute;
    uint8_t     hour;
    uint8_t     day;
    uint8_t     month;
    uint16_t    year;
} rtime_t;

// converts a Binary Coded Decimal (BDD) number to binary

// Parameters:  bcd -- The number in BCD format

// Returns:     an integer value for the value
extern unsigned int bcd_to_bin(unsigned int bcd);

// Read the system clock and return the time

// Parameters:  None

// Returns:     The time in rtime_t format
extern rtime_t read_system_clock();

// execute the rdtsc machine instruction

// Parameters:  None

// Returns:     the timestamp counter
extern uint64_t rdtsc(); // read the time stamp counter

#endif