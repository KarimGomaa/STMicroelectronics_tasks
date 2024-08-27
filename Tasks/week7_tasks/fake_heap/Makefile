# Compiler and flags
CC = gcc
CFLAGS = -static -g
RELEASE_FLAGS = -static
SHARED_FLAGS = -fPIC --shared
DEBUG_FLAGS = -g
OUTPUT_EXE = my_heap
OUTPUT_LIB = libhmm.so

# Default target
all: my_heap

# Default build rule for executable
my_heap: stress_test.c my_heap.c my_heap.h std_types.h
	$(CC) $(CFLAGS) -o $(OUTPUT_EXE) stress_test.c my_heap.c

# Release build rule (without debugging symbols)
release: stress_test.c my_heap.c my_heap.h std_types.h
	$(CC) $(RELEASE_FLAGS) -o $(OUTPUT_EXE) stress_test.c my_heap.c

# Shared library build rule
shared: my_heap.c my_heap.h std_types.h
	$(CC) $(SHARED_FLAGS) $(DEBUG_FLAGS) -o $(OUTPUT_LIB) my_heap.c

# Shared library with debugging symbols
sharedrelease: my_heap.c my_heap.h std_types.h
	$(CC) $(SHARED_FLAGS) -o $(OUTPUT_LIB) my_heap.c

# Clean up build artifacts
clean:
	rm -f $(OUTPUT_EXE) $(OUTPUT_LIB)
