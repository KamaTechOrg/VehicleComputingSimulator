# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /root/VehicleComputingSimulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/VehicleComputingSimulator/build

# Include any dependencies generated for this target.
include gui/test/CMakeFiles/UserInteractionTests.dir/depend.make

# Include the progress variables for this target.
include gui/test/CMakeFiles/UserInteractionTests.dir/progress.make

# Include the compile flags for this target's objects.
include gui/test/CMakeFiles/UserInteractionTests.dir/flags.make

gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o: gui/test/CMakeFiles/UserInteractionTests.dir/flags.make
gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o: gui/test/UserInteractionTests_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o"
	cd /root/VehicleComputingSimulator/build/gui/test && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o -c /root/VehicleComputingSimulator/build/gui/test/UserInteractionTests_autogen/mocs_compilation.cpp

gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.i"
	cd /root/VehicleComputingSimulator/build/gui/test && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/VehicleComputingSimulator/build/gui/test/UserInteractionTests_autogen/mocs_compilation.cpp > CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.i

gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.s"
	cd /root/VehicleComputingSimulator/build/gui/test && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/VehicleComputingSimulator/build/gui/test/UserInteractionTests_autogen/mocs_compilation.cpp -o CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.s

# Object files for target UserInteractionTests
UserInteractionTests_OBJECTS = \
"CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o"

# External object files for target UserInteractionTests
UserInteractionTests_EXTERNAL_OBJECTS =

gui/test/UserInteractionTests: gui/test/CMakeFiles/UserInteractionTests.dir/UserInteractionTests_autogen/mocs_compilation.cpp.o
gui/test/UserInteractionTests: gui/test/CMakeFiles/UserInteractionTests.dir/build.make
gui/test/UserInteractionTests: /usr/lib/x86_64-linux-gnu/libQt5Test.so.5.12.8
gui/test/UserInteractionTests: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.12.8
gui/test/UserInteractionTests: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.12.8
gui/test/UserInteractionTests: /usr/lib/x86_64-linux-gnu/libQt5Sql.so.5.12.8
gui/test/UserInteractionTests: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.12.8
gui/test/UserInteractionTests: gui/test/CMakeFiles/UserInteractionTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable UserInteractionTests"
	cd /root/VehicleComputingSimulator/build/gui/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/UserInteractionTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
gui/test/CMakeFiles/UserInteractionTests.dir/build: gui/test/UserInteractionTests

.PHONY : gui/test/CMakeFiles/UserInteractionTests.dir/build

gui/test/CMakeFiles/UserInteractionTests.dir/clean:
	cd /root/VehicleComputingSimulator/build/gui/test && $(CMAKE_COMMAND) -P CMakeFiles/UserInteractionTests.dir/cmake_clean.cmake
.PHONY : gui/test/CMakeFiles/UserInteractionTests.dir/clean

gui/test/CMakeFiles/UserInteractionTests.dir/depend:
	cd /root/VehicleComputingSimulator/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/VehicleComputingSimulator /root/VehicleComputingSimulator/gui/test /root/VehicleComputingSimulator/build /root/VehicleComputingSimulator/build/gui/test /root/VehicleComputingSimulator/build/gui/test/CMakeFiles/UserInteractionTests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gui/test/CMakeFiles/UserInteractionTests.dir/depend

