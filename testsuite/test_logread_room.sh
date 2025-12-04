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
output=$( ./logread -K GKgalleryKey99 -R -E Janet testlog.txt
          ./logread -K GKgalleryKey99 -R -G Bob testlog.txt 
          ./logread -K GKgalleryKey99 -R -G Charlie testlog.txt 
          ./logread -K GKgalleryKey99 -R -E John testlog.txt 
          ./logread -K GKgalleryKey99 -R -E Fred testlog.txt )

rm -f testlog.txt

expected="Janet is currently in Room: 7
Bob is currently in Room: 6
Charlie is currently in Room: 7
John is currently in Room: 22
Fred is currently in Room: Gallery"

if [[ "$output" != "$expected" ]]; then
    echo "[FAILED]"
    echo "=+= test_logread_room.sh Output =+="
    echo ""
    echo "Test logread room output did not match expected!" 
    echo "Expected: '$expected'"
    echo "Got: '$output'"
    echo ""
    echo "=+= end Output =+="
    exit 1
fi

echo "[PASSED]"
echo "=+= test_logread_room.sh Output =+="
echo ""
echo "Test logread room output passed."
echo ""
echo "=+= end Output =+="
exit 0