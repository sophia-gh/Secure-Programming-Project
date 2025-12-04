#!/bin/bash

./logappend -K GKgalleryKey99 -A -E Alice testElog.txt
./logappend -K GKgalleryKey99 -A -E Bob testElog.txt
./logappend -K GKgalleryKey99 -A -E Charlie testElog.txt
output=$(head -n 28 testElog.txt)  # don't read in lines with timestamps
rm -f testElog.txt

expected="employees in gallery line:Alice,Bob,Charlie,
guests in gallery line:
names in gallery line:Alice,Bob,Charlie,
names in room 1:
names in room 2:
names in room 3:
names in room 4:
names in room 5:
names in room 6:
names in room 7:
names in room 8:
names in room 9:
names in room 10:
names in room 11:
names in room 12:
names in room 13:
names in room 14:
names in room 15:
names in room 16:
names in room 17:
names in room 18:
names in room 19:
names in room 20:
names in room 21:
names in room 22:
names in room 23:
names in room 24:
names in room 25:"

if [[ "$output" != "$expected" ]]; then
    echo "[FAILED]"
    echo "=+= test_logappend_employee.sh Output =+="
    echo ""
    echo "Test logappend employee output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    echo ""
    echo "=+= end Output =+="
    exit 1
fi

echo "[PASSED]"
echo "=+= test_logappend_employee.sh Output =+="
echo ""
echo "Test logappend employee output passed."
echo ""
echo "=+= end Output =+="
exit 0