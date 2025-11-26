# Secure-Programming-Project
Build it, Break it, Fix it, logging system

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
The test_output.txt file conatins the output of each test, these outputs will explain why failing tests have failed. 