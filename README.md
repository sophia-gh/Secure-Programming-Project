# Secure-Programming-Project
Build it, Break it, Fix it, Logging system

Logging system implemented based on "Build it, Break it, Fix it" challenge outlined here: [outline](https://course.ece.cmu.edu/~ece732/s24/homework/bibifi/overview.html)

## Security Implementations
- Password hashing implemented with an external library: [bcrypt](https://github.com/hilch/Bcrypt.cpp). 
- File locking implmented with `flock()` and file descriptors to prevent multiple processses from accessing the same log file. 


# Usage
## Logappend
The logAppend program allows a user, with the proper key, to log a guest or employee's actions. The actions include: arriving in the gallery, arriving in a specific room, leaving a specific room, and leaving the gallery. A user must first arrive in the gallery to arrive in a subsequent room, and a user must leave an internal room to leave the gallery. We strayed from the instructions by auto-generating the timestamp rather than allowing users to specify the timestamp. The -B batch option is also not implemented. 

Allowed Use cases:
1. Guest or employee arrives in gallery before arriving in any internal room. 
    ```
    ./logappend -K <key> -A (-E <employee name> | -G <guest name>) <logfile name>
    ```
2. After a guest or employee is logged as ariving in the gallery room/ lobby, the same guest or employee can then be logged as entering an internal room. If a user attempts to log a guest or employee as entering when that guest or employee has not first entered the gallery, they are presented with a usage error. A guest or employee can also be logged as entering an internal room from any other room. 
    ```
    ./logappend -K <key> -A -R <room number> (-E <employee name> | -G <guest name>) <logfile name>
    ```
3. If a guest or employee is in an internal room, they must first leave the internal room before leaving the gallery. If a user attempts to log a leave operation with room number that the employee or guest is not in, an error is thrown and this action is blocked. 
    ```
    ./logappend -K <key> -L -R <room number> (-E <employee name> | -G <guest name>) <logfile name>
    ```
4. if a guest or employee is in the gallery room/ lobby they are able to leave the gallery.
    ```
    ./logappend -K <key> -L (-E <employee name> | -G <guest name>) <logfile name>
    ```
All actions are logged in the logfile with a timestamp to ensure data integrity and pinpoint usage errors should they occur. 

Usage: 
```cmd
Gallery Log Append: Appends arrival and leaving information to the specified gallery log file. Requires authentication key.
Usage: ./logappend -K <key> (-A | -L) -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>
Options:
       -K <key>            TEXT: authentication key to grant access to the log
       -A                  arrival (mutually exclusive with -L)
       -L                  leaving (mutually exclusive with -A) (must be in room to leave)
       -R <room number>    TEXT: (max: 25) (must be in gallery to arrive in room, must be in room to leave)
       -E <employee name>  TEXT: name (mutually exclusive with -G)
       -G <guest name>     TEXT: name (mutually exclusive with -E)
                           (Guest/Employee name must not contain numbers or special characters.)
       -h                  show this help message
```
## Logread
The logread program is implemented with two core features: read state, and read room. Read state allows authenticated users to view all guests and employees currently in the given log file. Read room allows authenticated users to view the room a specified guest or employee is currently in. 
Usage:
``` cmd
Gallery Log Read: Prints the current state of the specified log file. Requires authentication key.
Usage: ./logread
 print gallery state:                              -K <key> -S <logfileName>
 print room employee or guest is currently in:     -K <key> -R (-E <employeeName> | -G <guestName>) <logFileName>
  -K <key>
  -S
  -R <room> (optional)(must be followed by -E OR -G)
  -E <employee name> (optional)
  -G <guest name> (optional)
  -h    show this help
```

# Instructions to Run (in WSL or Linux Environments)
# 1. Compile Files (with Makefile)
``` cmd
    make 
```
*this creates the object files and executables for each program*
# 2. Run with -h to see usage
```cmd
    ./logappend -h
    ./logread -h 
```
# 3 clean with make file
```cmd
    make clean
```
*this removes all excutables and objects created with the 'make' command*

# Test Intsructions
# 1. Run all Test Files in `testsuite` (with Makefile)
ensure line endings are unix line endings, not windows line endings:
```cmd
    sed -i 's/\r$//' testsuite/*.sh
```
or using dos2unix if installed 
```cmd
    dos2unix testsuite/*.sh
```
Then run the make test command.
``` cmd
    make test
```
*this creates all executables and objects if not already created, and runs the tests*
# 2. View test Results
The output of `make test` should look like this. From the command line you can view the result of each test. To view the output simply locate the 
test_output.txt file which contains the output of each test run. 
``` cmd
Running test suite...

Test: testsuite/test_1.sh ................................[Pass/Fail result]
Test: testsuite/test_2.sh ................................[Pass/Fail result]
[...]
Test: testsuite/test_n.sh ................................[Pass/Fail result]

Done Testing!
View Output of each test in test_output.txt
```
The src/test_output.txt file conatins the output of each test, these outputs will explain why failing tests have failed. 

# 3 clean with make file
```cmd
    make clean
```
*this removes all excutables and objects created with the 'make test' or 'make command' command*