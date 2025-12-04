#!/bin/bash

output=$(./logappend -h 2>&1)
expected="Gallery Log Append: Appends arrival and leaving information to the specified gallery log file. Requires authentication key.
Usage: ./logappend -K <key> (-A | -L) -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>
Options:
       -K <key>            TEXT: authentication key to grant access to the log
       -A                  arrival (mutually exclusive with -L)
       -L                  leaving (mutually exclusive with -A) (must be in room to leave)
       -R <room number>    TEXT: (max: 25) (must be in gallery to arrive in room, must be in room to leave) 
       -E <employee name>  TEXT: name (mutually exclusive with -G)
       -G <guest name>     TEXT: name (mutually exclusive with -E)
                           (Guest/Employee name must not contain numbers or special characters.)
       -h                  show this help message"

diff_output=$(diff <(echo "$output") <(echo "$expected"))

if [[ "$output" != "$expected" ]]; then
    echo "[FAILED]"
    echo "=+= test_logappend_usage_output.sh Output =+="
    echo ""
    echo "Test logappend usage output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    echo ""
    echo "=+= end Output =+="
    exit 1
fi

echo "[PASSED]"
echo "=+= test_logappend_usage_output.sh Output =+="
echo ""
echo "Test logappend usage output passed."
echo ""
echo "=+= end Output =+="
exit 0