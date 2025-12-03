#!/bin/bash

./logappend -K GKgalleryKey99 -A -G Alice testGlog.txt
./logappend -K GKgalleryKey99 -A -G Bob testGlog.txt
./logappend -K GKgalleryKey99 -A -G Charlie testGlog.txt
output=$(head -n 28 testGlog.txt)  # don't read in lines with timestamps
rm -f testGlog.txt

expected="employees in gallery line:
guests in gallery line:Alice,Bob,Charlie,
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
    echo "=+= test_logappend_guest.sh Output =+="
    echo ""
    echo "Test logappend guest output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    echo ""
    echo "=+= end Output =+="
    exit 1
fi

echo "[PASSED]"
echo "=+= test_logappend_guest.sh Output =+="
echo ""
echo "Test logappend guest output passed."
echo ""
echo "=+= end Output =+="
exit 0