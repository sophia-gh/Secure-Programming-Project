#!/bin/bash

./logappend -K GKgalleryKey99 -A -G Alice testlog.txt
./logappend -K GKgalleryKey99 -A -G Bob testlog.txt
./logappend -K GKgalleryKey99 -A -G Charlie testlog.txt
./logappend -K GKgalleryKey99 -A -E Fred testlog.txt
./logappend -K GKgalleryKey99 -A -E Janet testlog.txt
./logappend -K GKgalleryKey99 -A -E John testlog.txt
./logappend -K GKgalleryKey99 -A -R 7 -E Janet testlog.txt
./logappend -K GKgalleryKey99 -A -R 22 -E John testlog.txt
./logappend -K GKgalleryKey99 -A -R 6 -G Bob testlog.txt
./logappend -K GKgalleryKey99 -A -R 7 -G Charlie testlog.txt
append_output=$(head -n 28 testlog.txt)  # don't read in lines with timestamps
output=$(./logread -K GKgalleryKey99 -S testlog.txt)
rm -f testlog.txt

expected="Employees in gallery: Fred,
Guests in gallery: Alice,
6: Bob
7: Charlie,Janet
22: John"

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