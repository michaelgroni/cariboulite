# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/lib/python3.7/dist-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /usr/local/lib/python3.7/dist-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build

# Include any dependencies generated for this target.
include CMakeFiles/datatypes.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/datatypes.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/datatypes.dir/flags.make

CMakeFiles/datatypes.dir/tsqueue.c.o: CMakeFiles/datatypes.dir/flags.make
CMakeFiles/datatypes.dir/tsqueue.c.o: ../tsqueue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/datatypes.dir/tsqueue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/datatypes.dir/tsqueue.c.o -c /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/tsqueue.c

CMakeFiles/datatypes.dir/tsqueue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/datatypes.dir/tsqueue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/tsqueue.c > CMakeFiles/datatypes.dir/tsqueue.c.i

CMakeFiles/datatypes.dir/tsqueue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/datatypes.dir/tsqueue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/tsqueue.c -o CMakeFiles/datatypes.dir/tsqueue.c.s

# Object files for target datatypes
datatypes_OBJECTS = \
"CMakeFiles/datatypes.dir/tsqueue.c.o"

# External object files for target datatypes
datatypes_EXTERNAL_OBJECTS =

libdatatypes.a: CMakeFiles/datatypes.dir/tsqueue.c.o
libdatatypes.a: CMakeFiles/datatypes.dir/build.make
libdatatypes.a: CMakeFiles/datatypes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libdatatypes.a"
	$(CMAKE_COMMAND) -P CMakeFiles/datatypes.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/datatypes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/datatypes.dir/build: libdatatypes.a

.PHONY : CMakeFiles/datatypes.dir/build

CMakeFiles/datatypes.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/datatypes.dir/cmake_clean.cmake
.PHONY : CMakeFiles/datatypes.dir/clean

CMakeFiles/datatypes.dir/depend:
	cd /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build /home/pi/projects/cariboulite/software/libcariboulite/src/datatypes/build/CMakeFiles/datatypes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/datatypes.dir/depend

