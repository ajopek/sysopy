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
CMAKE_SOURCE_DIR = /home/artur/sysopy/zestaw5/zad1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/artur/sysopy/zestaw5/zad1/build

# Include any dependencies generated for this target.
include CMakeFiles/zad1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zad1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zad1.dir/flags.make

CMakeFiles/zad1.dir/main.c.o: CMakeFiles/zad1.dir/flags.make
CMakeFiles/zad1.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artur/sysopy/zestaw5/zad1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/zad1.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/zad1.dir/main.c.o   -c /home/artur/sysopy/zestaw5/zad1/main.c

CMakeFiles/zad1.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zad1.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/artur/sysopy/zestaw5/zad1/main.c > CMakeFiles/zad1.dir/main.c.i

CMakeFiles/zad1.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zad1.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/artur/sysopy/zestaw5/zad1/main.c -o CMakeFiles/zad1.dir/main.c.s

CMakeFiles/zad1.dir/main.c.o.requires:

.PHONY : CMakeFiles/zad1.dir/main.c.o.requires

CMakeFiles/zad1.dir/main.c.o.provides: CMakeFiles/zad1.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/zad1.dir/build.make CMakeFiles/zad1.dir/main.c.o.provides.build
.PHONY : CMakeFiles/zad1.dir/main.c.o.provides

CMakeFiles/zad1.dir/main.c.o.provides.build: CMakeFiles/zad1.dir/main.c.o


# Object files for target zad1
zad1_OBJECTS = \
"CMakeFiles/zad1.dir/main.c.o"

# External object files for target zad1
zad1_EXTERNAL_OBJECTS =

zad1: CMakeFiles/zad1.dir/main.c.o
zad1: CMakeFiles/zad1.dir/build.make
zad1: CMakeFiles/zad1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/artur/sysopy/zestaw5/zad1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable zad1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zad1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zad1.dir/build: zad1

.PHONY : CMakeFiles/zad1.dir/build

CMakeFiles/zad1.dir/requires: CMakeFiles/zad1.dir/main.c.o.requires

.PHONY : CMakeFiles/zad1.dir/requires

CMakeFiles/zad1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zad1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zad1.dir/clean

CMakeFiles/zad1.dir/depend:
	cd /home/artur/sysopy/zestaw5/zad1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/artur/sysopy/zestaw5/zad1 /home/artur/sysopy/zestaw5/zad1 /home/artur/sysopy/zestaw5/zad1/build /home/artur/sysopy/zestaw5/zad1/build /home/artur/sysopy/zestaw5/zad1/build/CMakeFiles/zad1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zad1.dir/depend

