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
CMAKE_SOURCE_DIR = /home/tamar/Integration/VehicleComputingSimulator/main_bus

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tamar/Integration/VehicleComputingSimulator/main_bus/build

# Include any dependencies generated for this target.
include CMakeFiles/main_bus.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/main_bus.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/main_bus.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main_bus.dir/flags.make

CMakeFiles/main_bus.dir/main_bus.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/main_bus.cpp.o: ../main_bus.cpp
CMakeFiles/main_bus.dir/main_bus.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main_bus.dir/main_bus.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/main_bus.cpp.o -MF CMakeFiles/main_bus.dir/main_bus.cpp.o.d -o CMakeFiles/main_bus.dir/main_bus.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/main_bus/main_bus.cpp

CMakeFiles/main_bus.dir/main_bus.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/main_bus.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/main_bus/main_bus.cpp > CMakeFiles/main_bus.dir/main_bus.cpp.i

CMakeFiles/main_bus.dir/main_bus.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/main_bus.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/main_bus/main_bus.cpp -o CMakeFiles/main_bus.dir/main_bus.cpp.s

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o: /home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o -MF CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o.d -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp > CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.i

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.s

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o: /home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o -MF CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o.d -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp > CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.i

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.s

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o: /home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o -MF CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o.d -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp > CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.i

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.s

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o: /home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o -MF CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o.d -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp > CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.i

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.s

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o: CMakeFiles/main_bus.dir/flags.make
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o: /home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp
CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o: CMakeFiles/main_bus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o -MF CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o.d -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o -c /home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp > CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.i

CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp -o CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.s

# Object files for target main_bus
main_bus_OBJECTS = \
"CMakeFiles/main_bus.dir/main_bus.cpp.o" \
"CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o" \
"CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o" \
"CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o" \
"CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o" \
"CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o"

# External object files for target main_bus
main_bus_EXTERNAL_OBJECTS =

main_bus: CMakeFiles/main_bus.dir/main_bus.cpp.o
main_bus: CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/manager.cpp.o
main_bus: CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/server.cpp.o
main_bus: CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/packet.cpp.o
main_bus: CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/communication/src/message.cpp.o
main_bus: CMakeFiles/main_bus.dir/home/tamar/Integration/VehicleComputingSimulator/logger/logger.cpp.o
main_bus: CMakeFiles/main_bus.dir/build.make
main_bus: CMakeFiles/main_bus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable main_bus"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main_bus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main_bus.dir/build: main_bus
.PHONY : CMakeFiles/main_bus.dir/build

CMakeFiles/main_bus.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main_bus.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main_bus.dir/clean

CMakeFiles/main_bus.dir/depend:
	cd /home/tamar/Integration/VehicleComputingSimulator/main_bus/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tamar/Integration/VehicleComputingSimulator/main_bus /home/tamar/Integration/VehicleComputingSimulator/main_bus /home/tamar/Integration/VehicleComputingSimulator/main_bus/build /home/tamar/Integration/VehicleComputingSimulator/main_bus/build /home/tamar/Integration/VehicleComputingSimulator/main_bus/build/CMakeFiles/main_bus.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main_bus.dir/depend

