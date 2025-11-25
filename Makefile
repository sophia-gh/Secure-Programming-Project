#compiler flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS :=

# source files
SRCS = logappend.cpp logread.cpp authenticationHash.h
BCRYPT_SRCS = external/bcrypt/bcrypt.cpp external/bcrypt/blowfish.cpp

# object files
OBJS = $(SRCS:.cpp=.o)
BCRYPT_OBJS = $(BCRYPT_SRCS:.cpp=.o)

# targets ------------------------------------------------------------------
TARGETS = logappend logread
.PHONY: all clean

all: $(TARGETS)

#make executables individually
logappend: logappend.o  $(BCRYPT_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

logread: logread.o $(BCRYPT_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

#make object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)
	rm -f external/bcrypt/*.o

#testing
TESTS := $(wildcard testsuite/*.sh)

.PHONY: test
test: all $(TESTS)
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		bash $$test || exit 1; \
	done
	@echo "All tests passed!"

