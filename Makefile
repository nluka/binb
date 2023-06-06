CXX = gcc
CXXFLAGS = -Werror -Wall -Wextra -Wconversion -pedantic -Wno-unused-but-set-parameter

release:
	$(CXX) $(CXXFLAGS) -O2 src/main.c -o binb

debug:
	$(CXX) $(CXXFLAGS) -g src/main.c -o binb
