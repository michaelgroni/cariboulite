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
CMAKE_SOURCE_DIR = /home/pi/projects/cariboulite/software/libcariboulite

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/projects/cariboulite/software/libcariboulite/build

# Include any dependencies generated for this target.
include CMakeFiles/ice40programmer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ice40programmer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ice40programmer.dir/flags.make

CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o: CMakeFiles/ice40programmer.dir/flags.make
CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o: ../test/ice40_programmer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o -c /home/pi/projects/cariboulite/software/libcariboulite/test/ice40_programmer.c

CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/projects/cariboulite/software/libcariboulite/test/ice40_programmer.c > CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.i

CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/projects/cariboulite/software/libcariboulite/test/ice40_programmer.c -o CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.s

# Object files for target ice40programmer
ice40programmer_OBJECTS = \
"CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o"

# External object files for target ice40programmer
ice40programmer_EXTERNAL_OBJECTS =

test/ice40programmer: CMakeFiles/ice40programmer.dir/test/ice40_programmer.c.o
test/ice40programmer: CMakeFiles/ice40programmer.dir/build.make
test/ice40programmer: libcariboulite.a
test/ice40programmer: src/datatypes/libdatatypes.a
test/ice40programmer: src/ustimer/libustimer.a
test/ice40programmer: src/caribou_fpga/libcaribou_fpga.a
test/ice40programmer: src/at86rf215/libat86rf215.a
test/ice40programmer: src/rffc507x/librffc507x.a
test/ice40programmer: src/caribou_smi/libcaribou_smi.a
test/ice40programmer: src/latticeice40/liblatticeice40.a
test/ice40programmer: src/io_utils/libio_utils.a
test/ice40programmer: src/zf_log/libzf_log.a
test/ice40programmer: CMakeFiles/ice40programmer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test/ice40programmer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ice40programmer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ice40programmer.dir/build: test/ice40programmer

.PHONY : CMakeFiles/ice40programmer.dir/build

CMakeFiles/ice40programmer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ice40programmer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ice40programmer.dir/clean

CMakeFiles/ice40programmer.dir/depend:
	cd /home/pi/projects/cariboulite/software/libcariboulite/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/projects/cariboulite/software/libcariboulite /home/pi/projects/cariboulite/software/libcariboulite /home/pi/projects/cariboulite/software/libcariboulite/build /home/pi/projects/cariboulite/software/libcariboulite/build /home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles/ice40programmer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ice40programmer.dir/depend

