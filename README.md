# Secure-Programming-Project
Build it, Break it, Fix it, logging system

# Instructions to Run (in WSL or Linux Environments)
# 1. Compile Files (with Makefile)
``` cmd
    make all
```
*this creates the object files and executables for each program*
# 1.1 compile files without MakeFile
```cmd
    g++ -std=c++17 -Wall -Wextra -O2 logappend.cpp -o logappend
    g++ -std=c++17 -Wall -Wextra -O2 logread.cpp -o logread
```
# 2. Run with -h to see usage (still some issues here with arg parsing and input checking)
```cmd
    ./logappend -h
    ./logread -h 
```
# 3 clean with make file
```cmd
    make clean
```