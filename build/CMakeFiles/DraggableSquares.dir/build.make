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

# Include any dependencies generated for this target.
include CMakeFiles/DraggableSquares.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/DraggableSquares.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/DraggableSquares.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/DraggableSquares.dir/flags.make

CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o: DraggableSquares_autogen/mocs_compilation.cpp
CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o -MF CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/build/DraggableSquares_autogen/mocs_compilation.cpp

CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/build/DraggableSquares_autogen/mocs_compilation.cpp > CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.i

CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/build/DraggableSquares_autogen/mocs_compilation.cpp -o CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.s

CMakeFiles/DraggableSquares.dir/src/main.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/DraggableSquares.dir/src/main.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/DraggableSquares.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/src/main.cpp.o -MF CMakeFiles/DraggableSquares.dir/src/main.cpp.o.d -o CMakeFiles/DraggableSquares.dir/src/main.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/src/main.cpp

CMakeFiles/DraggableSquares.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/src/main.cpp > CMakeFiles/DraggableSquares.dir/src/main.cpp.i

CMakeFiles/DraggableSquares.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/src/main.cpp -o CMakeFiles/DraggableSquares.dir/src/main.cpp.s

CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o: ../src/draggable_square.cpp
CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o -MF CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o.d -o CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/src/draggable_square.cpp

CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/src/draggable_square.cpp > CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.i

CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/src/draggable_square.cpp -o CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.s

CMakeFiles/DraggableSquares.dir/src/process.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/src/process.cpp.o: ../src/process.cpp
CMakeFiles/DraggableSquares.dir/src/process.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/DraggableSquares.dir/src/process.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/src/process.cpp.o -MF CMakeFiles/DraggableSquares.dir/src/process.cpp.o.d -o CMakeFiles/DraggableSquares.dir/src/process.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/src/process.cpp

CMakeFiles/DraggableSquares.dir/src/process.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/src/process.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/src/process.cpp > CMakeFiles/DraggableSquares.dir/src/process.cpp.i

CMakeFiles/DraggableSquares.dir/src/process.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/src/process.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/src/process.cpp -o CMakeFiles/DraggableSquares.dir/src/process.cpp.s

CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o: ../src/process_dialog.cpp
CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o -MF CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o.d -o CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/src/process_dialog.cpp

CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/src/process_dialog.cpp > CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.i

CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/src/process_dialog.cpp -o CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.s

CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o: CMakeFiles/DraggableSquares.dir/flags.make
CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o: ../src/main_window.cpp
CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o: CMakeFiles/DraggableSquares.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o -MF CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o.d -o CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o -c /home/chaya/git3/VehicleComputingSimulator/src/main_window.cpp

CMakeFiles/DraggableSquares.dir/src/main_window.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DraggableSquares.dir/src/main_window.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chaya/git3/VehicleComputingSimulator/src/main_window.cpp > CMakeFiles/DraggableSquares.dir/src/main_window.cpp.i

CMakeFiles/DraggableSquares.dir/src/main_window.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DraggableSquares.dir/src/main_window.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chaya/git3/VehicleComputingSimulator/src/main_window.cpp -o CMakeFiles/DraggableSquares.dir/src/main_window.cpp.s

# Object files for target DraggableSquares
DraggableSquares_OBJECTS = \
"CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/DraggableSquares.dir/src/main.cpp.o" \
"CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o" \
"CMakeFiles/DraggableSquares.dir/src/process.cpp.o" \
"CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o" \
"CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o"

# External object files for target DraggableSquares
DraggableSquares_EXTERNAL_OBJECTS =

DraggableSquares: CMakeFiles/DraggableSquares.dir/DraggableSquares_autogen/mocs_compilation.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/src/main.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/src/draggable_square.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/src/process.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/src/process_dialog.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/src/main_window.cpp.o
DraggableSquares: CMakeFiles/DraggableSquares.dir/build.make
DraggableSquares: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.15.3
DraggableSquares: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.15.3
DraggableSquares: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.15.3
DraggableSquares: CMakeFiles/DraggableSquares.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable DraggableSquares"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/DraggableSquares.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/DraggableSquares.dir/build: DraggableSquares
.PHONY : CMakeFiles/DraggableSquares.dir/build

CMakeFiles/DraggableSquares.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/DraggableSquares.dir/cmake_clean.cmake
.PHONY : CMakeFiles/DraggableSquares.dir/clean

CMakeFiles/DraggableSquares.dir/depend:
	cd /home/chaya/git3/VehicleComputingSimulator/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chaya/git3/VehicleComputingSimulator /home/chaya/git3/VehicleComputingSimulator /home/chaya/git3/VehicleComputingSimulator/build /home/chaya/git3/VehicleComputingSimulator/build /home/chaya/git3/VehicleComputingSimulator/build/CMakeFiles/DraggableSquares.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/DraggableSquares.dir/depend

