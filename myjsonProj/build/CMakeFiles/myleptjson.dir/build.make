# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/wg/GitHub/Project/myjsonProj

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wg/GitHub/Project/myjsonProj/build

# Include any dependencies generated for this target.
include CMakeFiles/myleptjson.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myleptjson.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myleptjson.dir/flags.make

CMakeFiles/myleptjson.dir/myleptjson.c.o: CMakeFiles/myleptjson.dir/flags.make
CMakeFiles/myleptjson.dir/myleptjson.c.o: ../myleptjson.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wg/GitHub/Project/myjsonProj/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/myleptjson.dir/myleptjson.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/myleptjson.dir/myleptjson.c.o   -c /home/wg/GitHub/Project/myjsonProj/myleptjson.c

CMakeFiles/myleptjson.dir/myleptjson.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myleptjson.dir/myleptjson.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/wg/GitHub/Project/myjsonProj/myleptjson.c > CMakeFiles/myleptjson.dir/myleptjson.c.i

CMakeFiles/myleptjson.dir/myleptjson.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myleptjson.dir/myleptjson.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/wg/GitHub/Project/myjsonProj/myleptjson.c -o CMakeFiles/myleptjson.dir/myleptjson.c.s

CMakeFiles/myleptjson.dir/myleptjson.c.o.requires:
.PHONY : CMakeFiles/myleptjson.dir/myleptjson.c.o.requires

CMakeFiles/myleptjson.dir/myleptjson.c.o.provides: CMakeFiles/myleptjson.dir/myleptjson.c.o.requires
	$(MAKE) -f CMakeFiles/myleptjson.dir/build.make CMakeFiles/myleptjson.dir/myleptjson.c.o.provides.build
.PHONY : CMakeFiles/myleptjson.dir/myleptjson.c.o.provides

CMakeFiles/myleptjson.dir/myleptjson.c.o.provides.build: CMakeFiles/myleptjson.dir/myleptjson.c.o

# Object files for target myleptjson
myleptjson_OBJECTS = \
"CMakeFiles/myleptjson.dir/myleptjson.c.o"

# External object files for target myleptjson
myleptjson_EXTERNAL_OBJECTS =

libmyleptjson.a: CMakeFiles/myleptjson.dir/myleptjson.c.o
libmyleptjson.a: CMakeFiles/myleptjson.dir/build.make
libmyleptjson.a: CMakeFiles/myleptjson.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libmyleptjson.a"
	$(CMAKE_COMMAND) -P CMakeFiles/myleptjson.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myleptjson.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myleptjson.dir/build: libmyleptjson.a
.PHONY : CMakeFiles/myleptjson.dir/build

CMakeFiles/myleptjson.dir/requires: CMakeFiles/myleptjson.dir/myleptjson.c.o.requires
.PHONY : CMakeFiles/myleptjson.dir/requires

CMakeFiles/myleptjson.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myleptjson.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myleptjson.dir/clean

CMakeFiles/myleptjson.dir/depend:
	cd /home/wg/GitHub/Project/myjsonProj/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wg/GitHub/Project/myjsonProj /home/wg/GitHub/Project/myjsonProj /home/wg/GitHub/Project/myjsonProj/build /home/wg/GitHub/Project/myjsonProj/build /home/wg/GitHub/Project/myjsonProj/build/CMakeFiles/myleptjson.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myleptjson.dir/depend

