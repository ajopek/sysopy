# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/artur/sysopy/zestaw4/zad2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/artur/sysopy/zestaw4/zad2/build

# Include any dependencies generated for this target.
include CMakeFiles/main2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main2.dir/flags.make

CMakeFiles/main2.dir/main2.c.o: CMakeFiles/main2.dir/flags.make
CMakeFiles/main2.dir/main2.c.o: ../main2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artur/sysopy/zestaw4/zad2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/main2.dir/main2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/main2.dir/main2.c.o   -c /home/artur/sysopy/zestaw4/zad2/main2.c

CMakeFiles/main2.dir/main2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/main2.dir/main2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/artur/sysopy/zestaw4/zad2/main2.c > CMakeFiles/main2.dir/main2.c.i

CMakeFiles/main2.dir/main2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/main2.dir/main2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/artur/sysopy/zestaw4/zad2/main2.c -o CMakeFiles/main2.dir/main2.c.s

CMakeFiles/main2.dir/main2.c.o.requires:

.PHONY : CMakeFiles/main2.dir/main2.c.o.requires

CMakeFiles/main2.dir/main2.c.o.provides: CMakeFiles/main2.dir/main2.c.o.requires
	$(MAKE) -f CMakeFiles/main2.dir/build.make CMakeFiles/main2.dir/main2.c.o.provides.build
.PHONY : CMakeFiles/main2.dir/main2.c.o.provides

CMakeFiles/main2.dir/main2.c.o.provides.build: CMakeFiles/main2.dir/main2.c.o


# Object files for target main2
main2_OBJECTS = \
"CMakeFiles/main2.dir/main2.c.o"

# External object files for target main2
main2_EXTERNAL_OBJECTS =

main2: CMakeFiles/main2.dir/main2.c.o
main2: CMakeFiles/main2.dir/build.make
main2: CMakeFiles/main2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/artur/sysopy/zestaw4/zad2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable main2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main2.dir/build: main2

.PHONY : CMakeFiles/main2.dir/build

CMakeFiles/main2.dir/requires: CMakeFiles/main2.dir/main2.c.o.requires

.PHONY : CMakeFiles/main2.dir/requires

CMakeFiles/main2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main2.dir/clean

CMakeFiles/main2.dir/depend:
	cd /home/artur/sysopy/zestaw4/zad2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/artur/sysopy/zestaw4/zad2 /home/artur/sysopy/zestaw4/zad2 /home/artur/sysopy/zestaw4/zad2/build /home/artur/sysopy/zestaw4/zad2/build /home/artur/sysopy/zestaw4/zad2/build/CMakeFiles/main2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main2.dir/depend

