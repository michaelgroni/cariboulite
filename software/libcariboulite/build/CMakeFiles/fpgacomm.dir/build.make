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
include CMakeFiles/fpgacomm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/fpgacomm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fpgacomm.dir/flags.make

CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o: CMakeFiles/fpgacomm.dir/flags.make
CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o: ../test/fpga_comm_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o -c /home/pi/projects/cariboulite/software/libcariboulite/test/fpga_comm_test.c

CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/projects/cariboulite/software/libcariboulite/test/fpga_comm_test.c > CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.i

CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/projects/cariboulite/software/libcariboulite/test/fpga_comm_test.c -o CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.s

# Object files for target fpgacomm
fpgacomm_OBJECTS = \
"CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o"

# External object files for target fpgacomm
fpgacomm_EXTERNAL_OBJECTS =

test/fpgacomm: CMakeFiles/fpgacomm.dir/test/fpga_comm_test.c.o
test/fpgacomm: CMakeFiles/fpgacomm.dir/build.make
test/fpgacomm: libcariboulite.a
test/fpgacomm: src/datatypes/libdatatypes.a
test/fpgacomm: src/ustimer/libustimer.a
test/fpgacomm: src/caribou_fpga/libcaribou_fpga.a
test/fpgacomm: src/caribou_smi/libcaribou_smi.a
test/fpgacomm: src/latticeice40/liblatticeice40.a
test/fpgacomm: src/io_utils/libio_utils.a
test/fpgacomm: CMakeFiles/fpgacomm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test/fpgacomm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fpgacomm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fpgacomm.dir/build: test/fpgacomm

.PHONY : CMakeFiles/fpgacomm.dir/build

CMakeFiles/fpgacomm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fpgacomm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fpgacomm.dir/clean

CMakeFiles/fpgacomm.dir/depend:
	cd /home/pi/projects/cariboulite/software/libcariboulite/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/projects/cariboulite/software/libcariboulite /home/pi/projects/cariboulite/software/libcariboulite /home/pi/projects/cariboulite/software/libcariboulite/build /home/pi/projects/cariboulite/software/libcariboulite/build /home/pi/projects/cariboulite/software/libcariboulite/build/CMakeFiles/fpgacomm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/fpgacomm.dir/depend

