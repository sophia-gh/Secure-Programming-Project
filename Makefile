#compiler flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS :=

#directories
SRCS = logappend.cpp logread.cpp 
OBJS = $(SRCS:.cpp=.o)
TARGETS = logappend logread 
.PHONY: all clean

all: $(TARGETS)

#make executables individually
logappend: logappend.o
	$(CXX) $(LDFLAGS) -o $@ $^

logread: logread.o
	$(CXX) $(LDFLAGS) -o $@ $^

#make object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)

#testing
TESTS := $(wildcard testsuite/*.sh)

.PHONY: test
test: all $(TESTS)
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		bash $$test || exit 1; \
	done
	@echo "All tests passed!"

