#!/bin/bash

output=$(./logread -h 2>&1)
expected="Usage: ./logread print gallery state:                    -K <key> -S <logfileName>
 print list of people in specified room: -K <key> -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>
  -K <key>
  -S
  -R <room> (optional)(must be followed by -E OR -G)
  -E <employee name> (optional)
  -G <guest name> (optional)
  -h    show this help"

diff_output=$(diff <(echo "$output") <(echo "$expected"))

if [[ "$output" != "$expected" ]]; then
    echo "Test -h help output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    exit 1
fi

echo "Test -h help output passed."
exit 0