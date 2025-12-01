# compiler flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS :=

# source files
SRCS = logappend.cpp logread.cpp
BCRYPT_SRCS = external/bcrypt/bcrypt.cpp external/bcrypt/blowfish.cpp

# object files
OBJS = $(SRCS:.cpp=.o)
BCRYPT_OBJS = $(BCRYPT_SRCS:.cpp=.o)

# targets ------------------------------------------------------------------
TARGETS = logappend logread
.PHONY: all clean

all: $(TARGETS)
	@echo "\033[38;5;42m[Build Complete]\033[0m"

# executables
logappend: logappend.o $(BCRYPT_OBJS)
	@echo "\033[38;5;190m[Linking]\033[0m           $@"
	@$(CXX) $(LDFLAGS) -o $@ $^

logread: logread.o $(BCRYPT_OBJS)
	@echo "\033[38;5;190m[Linking]\033[0m           $@"
	@$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	@echo "\033[38;5;42m[Building File]\033[0m     $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BCRYPT_OBJS) $(TARGETS) test_output.txt

# tests --------------------------------------------------------------------
TESTS := $(wildcard testsuite/*.sh)

.PHONY: test
test: all
	@echo "\033[0;34mRunning test suite...\033[0m\n"
	@rm -f test_output.txt
	@touch test_output.txt
	@printf "==================== Test Suite Output ====================\n" >> test_output.txt
	@for test in $(TESTS); do \
		printf "\n-------------------------------------------------------------------------------------------------------------------------------------\n" >> test_output.txt; \
		printf "Test: %s " "$$test" | tee -a test_output.txt; \
		if bash $$test >> test_output.txt 2>&1; then \
			printf "................................\033[0;32m[Passed]\033[0m\n"; \
		else \
			printf "................................\033[0;31m[Failed]\033[0m\n"; \
		fi; \
	done

	@echo "\n\033[0;34mDone Testing!\033[0m"
	@echo "\033[0;34mView Output of each test in test_output.txt\033[0m"