# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chaya/git3/VehicleComputingSimulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chaya/git3/VehicleComputingSimulator/build

# Utility rule file for DraggableSquares_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/DraggableSquares_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/DraggableSquares_autogen.dir/progress.make

CMakeFiles/DraggableSquares_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target DraggableSquares"
	/usr/bin/cmake -E cmake_autogen /home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles/DraggableSquares_autogen.dir/AutogenInfo.json ""

DraggableSquares_autogen: CMakeFiles/DraggableSquares_autogen
DraggableSquares_autogen: CMakeFiles/DraggableSquares_autogen.dir/build.make
.PHONY : DraggableSquares_autogen

# Rule to build all files generated by this target.
CMakeFiles/DraggableSquares_autogen.dir/build: DraggableSquares_autogen
.PHONY : CMakeFiles/DraggableSquares_autogen.dir/build

CMakeFiles/DraggableSquares_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/DraggableSquares_autogen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/DraggableSquares_autogen.dir/clean

CMakeFiles/DraggableSquares_autogen.dir/depend:
	cd /home/chaya/git3/VehicleComputingSimulator/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chaya/git3/VehicleComputingSimulator /home/chaya/git3/VehicleComputingSimulator /home/chaya/git3/VehicleComputingSimulator/build /home/chaya/git3/VehicleComputingSimulator/build /home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles/DraggableSquares_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/DraggableSquares_autogen.dir/depend

