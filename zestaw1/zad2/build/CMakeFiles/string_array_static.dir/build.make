# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/artur/sysopy/zestaw1/zad2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/artur/sysopy/zestaw1/zad2/build

# Include any dependencies generated for this target.
include CMakeFiles/string_array_static.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/string_array_static.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/string_array_static.dir/flags.make

CMakeFiles/string_array_static.dir/string_array_lib.c.o: CMakeFiles/string_array_static.dir/flags.make
CMakeFiles/string_array_static.dir/string_array_lib.c.o: ../string_array_lib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artur/sysopy/zestaw1/zad2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/string_array_static.dir/string_array_lib.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/string_array_static.dir/string_array_lib.c.o   -c /home/artur/sysopy/zestaw1/zad2/string_array_lib.c

CMakeFiles/string_array_static.dir/string_array_lib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/string_array_static.dir/string_array_lib.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/artur/sysopy/zestaw1/zad2/string_array_lib.c > CMakeFiles/string_array_static.dir/string_array_lib.c.i

CMakeFiles/string_array_static.dir/string_array_lib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/string_array_static.dir/string_array_lib.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/artur/sysopy/zestaw1/zad2/string_array_lib.c -o CMakeFiles/string_array_static.dir/string_array_lib.c.s

CMakeFiles/string_array_static.dir/string_array_lib.c.o.requires:

.PHONY : CMakeFiles/string_array_static.dir/string_array_lib.c.o.requires

CMakeFiles/string_array_static.dir/string_array_lib.c.o.provides: CMakeFiles/string_array_static.dir/string_array_lib.c.o.requires
	$(MAKE) -f CMakeFiles/string_array_static.dir/build.make CMakeFiles/string_array_static.dir/string_array_lib.c.o.provides.build
.PHONY : CMakeFiles/string_array_static.dir/string_array_lib.c.o.provides

CMakeFiles/string_array_static.dir/string_array_lib.c.o.provides.build: CMakeFiles/string_array_static.dir/string_array_lib.c.o


# Object files for target string_array_static
string_array_static_OBJECTS = \
"CMakeFiles/string_array_static.dir/string_array_lib.c.o"

# External object files for target string_array_static
string_array_static_EXTERNAL_OBJECTS =

libstring_array_static.a: CMakeFiles/string_array_static.dir/string_array_lib.c.o
libstring_array_static.a: CMakeFiles/string_array_static.dir/build.make
libstring_array_static.a: CMakeFiles/string_array_static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/artur/sysopy/zestaw1/zad2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libstring_array_static.a"
	$(CMAKE_COMMAND) -P CMakeFiles/string_array_static.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/string_array_static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/string_array_static.dir/build: libstring_array_static.a

.PHONY : CMakeFiles/string_array_static.dir/build

CMakeFiles/string_array_static.dir/requires: CMakeFiles/string_array_static.dir/string_array_lib.c.o.requires

.PHONY : CMakeFiles/string_array_static.dir/requires

CMakeFiles/string_array_static.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/string_array_static.dir/cmake_clean.cmake
.PHONY : CMakeFiles/string_array_static.dir/clean

CMakeFiles/string_array_static.dir/depend:
	cd /home/artur/sysopy/zestaw1/zad2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/artur/sysopy/zestaw1/zad2 /home/artur/sysopy/zestaw1/zad2 /home/artur/sysopy/zestaw1/zad2/build /home/artur/sysopy/zestaw1/zad2/build /home/artur/sysopy/zestaw1/zad2/build/CMakeFiles/string_array_static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/string_array_static.dir/depend

