#!/bin/bash

output=$(./logread -h 2>&1)
expected="Gallery Log Read: Prints the current state of the specified log file. Requires authentication key.
Usage: ./logread print gallery state:                              -K <key> -S <logfileName>
 print list of people currently in specified room: -K <key> -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>
  -K <key>
  -S
  -R <room> (optional)(must be followed by -E OR -G)
  -E <employee name> (optional)
  -G <guest name> (optional)
  -h    show this help"

diff_output=$(diff <(echo "$output") <(echo "$expected"))
 
if [[ "$output" != "$expected" ]]; then
    echo "[FAILED]"
    echo "=+= test_logread_usage_output.sh Output =+="
    echo ""
    echo "Test logread usage output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    echo ""
    echo "=+= end Output =+="
    exit 1
fi
echo "[PASSED]"
echo "=+= test_logread_usage_output.sh Output =+="
echo ""
echo "Test logread usage output passed."
echo ""
echo "=+= end Output =+="
exit 0