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
CMAKE_SOURCE_DIR = /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build

# Include any dependencies generated for this target.
include CMakeFiles/dummy_program.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/dummy_program.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/dummy_program.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dummy_program.dir/flags.make

CMakeFiles/dummy_program.dir/main.cpp.o: CMakeFiles/dummy_program.dir/flags.make
CMakeFiles/dummy_program.dir/main.cpp.o: ../main.cpp
CMakeFiles/dummy_program.dir/main.cpp.o: CMakeFiles/dummy_program.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dummy_program.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dummy_program.dir/main.cpp.o -MF CMakeFiles/dummy_program.dir/main.cpp.o.d -o CMakeFiles/dummy_program.dir/main.cpp.o -c /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/main.cpp

CMakeFiles/dummy_program.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dummy_program.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/main.cpp > CMakeFiles/dummy_program.dir/main.cpp.i

CMakeFiles/dummy_program.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dummy_program.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/main.cpp -o CMakeFiles/dummy_program.dir/main.cpp.s

# Object files for target dummy_program
dummy_program_OBJECTS = \
"CMakeFiles/dummy_program.dir/main.cpp.o"

# External object files for target dummy_program
dummy_program_EXTERNAL_OBJECTS =

dummy_program: CMakeFiles/dummy_program.dir/main.cpp.o
dummy_program: CMakeFiles/dummy_program.dir/build.make
dummy_program: CMakeFiles/dummy_program.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable dummy_program"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dummy_program.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dummy_program.dir/build: dummy_program
.PHONY : CMakeFiles/dummy_program.dir/build

CMakeFiles/dummy_program.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dummy_program.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dummy_program.dir/clean

CMakeFiles/dummy_program.dir/depend:
	cd /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1 /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1 /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build /home/tamar/chayaProject/VehicleComputingSimulator/src/dummy_program1/build/CMakeFiles/dummy_program.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dummy_program.dir/depend

