# Use clang as the C compiler
CC = clang
# Flags to pass to clang:
# -Iinclude tells clang to look for #include files in the "include" folder
# -Wall turns on all warnings
# -g adds filenames and line numbers to the executable for useful stack traces
# -fno-omit-frame-pointer allows stack traces to be generated
#   (take CS 24 for a full explanation)
# -fsanitize=address enables asan
CFLAGS = -Iinclude -Wall -g -fno-omit-frame-pointer -fsanitize=address
# Compiler flag that links the program with the math library
LIB_MATH = -lm
# Compiler flags that link the program with the math and SDL libraries.
# Note that $(...) substitutes a variable's value, so this line is equivalent to
# LIBS = -lm -lSDL2 -lSDL2_gfx
LIBS = $(LIB_MATH) -lSDL2 -lSDL2_gfx -lSDL2_mixer -lSDL2_ttf -lSDL2_image

# List of demo programs
DEMOS = finalgame test
# List of C files in "libraries" that we provide
STAFF_LIBS = test_util sdl_wrapper SDL_ttf
# List of C files in "libraries" that you will write
STUDENT_LIBS = vector list \
	color body scene \
	polygon forces star collision

# List of compiled .o files corresponding to STUDENT_LIBS, e.g. "out/vector.o".
# Don't worry about the syntax; it's just adding "out/" to the start
# and ".o" to the end of each value in STUDENT_LIBS.
STUDENT_OBJS = $(addprefix out/,$(STUDENT_LIBS:=.o))
# List of demo executables, i.e. "bin/bounce".
DEMO_BINS = $(addprefix bin/,$(DEMOS))
# All executables (the concatenation of TEST_BINS and DEMO_BINS)
BINS = $(DEMO_BINS)

# The first Make rule. It is relatively simple:
# "To build 'all', make sure all files in BINS are up to date."
# You can execute this rule by running the command "make all", or just "make".
all: $(BINS)

# Any .o file in "out" is built from the corresponding C file.
# Although .c files can be directly compiled into an executable, first building
# .o files reduces the amount of work needed to rebuild the executable.
# For example, if only vector.c was modified since the last build, only vector.o
# gets recompiled, and clang reuses the other .o files to build the executable.
#
# "%" means "any string".
# Unlike "all", this target has a build command.
# "$^" is a special variable meaning "the source files"
# and $@ means "the target file", so the command tells clang
# to compile the source C file into the target .o file.
out/%.o: library/%.c # source file may be found in "library"
	$(CC) -c $(CFLAGS) $^ -o $@
out/%.o: tests/%.c # or "tests"
	$(CC) -c $(CFLAGS) $^ -o $@
out/%.o: tests/student/%.c # or "tests"
	$(CC) -c $(CFLAGS) $^ -o $@

out/demo-%.o: demo/%.c # or "demo"; in this case, add "demo-" to the .o filename
	$(CC) -c $(CFLAGS) $^ -o $@

# Builds the demos by linking the necessary .o files.
# Unlike the out/%.o rule, this uses the LIBS flags and omits the -c flag,
# since it is building a full executable.
bin/%: out/demo-%.o out/sdl_wrapper.o $(STUDENT_OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@



# Removes all compiled files. "out/*" matches all files in the "out" directory
# and "bin/*" does the same for the "bin" directory.
# "rm" deletes the files; "-f" means "succeed even if no files were removed".
# Note that this target has no sources, which is perfectly valid.
clean:
	rm -f out/* bin/*

# This special rule tells Make that "all", "clean", and "test" are rules
# that don't build a file.
.PHONY: all clean test
# Tells Make not to delete the .o files after the executable is built
.PRECIOUS: out/%.o out/demo-%.o
