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
CMAKE_SOURCE_DIR = /home/csnesummer/Documents/cropIms

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/csnesummer/Documents/cropIms/build

# Include any dependencies generated for this target.
include CMakeFiles/cropIms.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cropIms.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cropIms.dir/flags.make

CMakeFiles/cropIms.dir/main.o: CMakeFiles/cropIms.dir/flags.make
CMakeFiles/cropIms.dir/main.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/csnesummer/Documents/cropIms/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cropIms.dir/main.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cropIms.dir/main.o -c /home/csnesummer/Documents/cropIms/main.cpp

CMakeFiles/cropIms.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cropIms.dir/main.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/csnesummer/Documents/cropIms/main.cpp > CMakeFiles/cropIms.dir/main.i

CMakeFiles/cropIms.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cropIms.dir/main.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/csnesummer/Documents/cropIms/main.cpp -o CMakeFiles/cropIms.dir/main.s

CMakeFiles/cropIms.dir/main.o.requires:

.PHONY : CMakeFiles/cropIms.dir/main.o.requires

CMakeFiles/cropIms.dir/main.o.provides: CMakeFiles/cropIms.dir/main.o.requires
	$(MAKE) -f CMakeFiles/cropIms.dir/build.make CMakeFiles/cropIms.dir/main.o.provides.build
.PHONY : CMakeFiles/cropIms.dir/main.o.provides

CMakeFiles/cropIms.dir/main.o.provides.build: CMakeFiles/cropIms.dir/main.o


# Object files for target cropIms
cropIms_OBJECTS = \
"CMakeFiles/cropIms.dir/main.o"

# External object files for target cropIms
cropIms_EXTERNAL_OBJECTS =

cropIms: CMakeFiles/cropIms.dir/main.o
cropIms: CMakeFiles/cropIms.dir/build.make
cropIms: /usr/local/lib/libopencv_highgui.so.3.1.0
cropIms: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
cropIms: /usr/local/lib/libopencv_imgproc.so.3.1.0
cropIms: /usr/local/lib/libopencv_core.so.3.1.0
cropIms: CMakeFiles/cropIms.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/csnesummer/Documents/cropIms/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cropIms"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cropIms.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cropIms.dir/build: cropIms

.PHONY : CMakeFiles/cropIms.dir/build

CMakeFiles/cropIms.dir/requires: CMakeFiles/cropIms.dir/main.o.requires

.PHONY : CMakeFiles/cropIms.dir/requires

CMakeFiles/cropIms.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cropIms.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cropIms.dir/clean

CMakeFiles/cropIms.dir/depend:
	cd /home/csnesummer/Documents/cropIms/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/csnesummer/Documents/cropIms /home/csnesummer/Documents/cropIms /home/csnesummer/Documents/cropIms/build /home/csnesummer/Documents/cropIms/build /home/csnesummer/Documents/cropIms/build/CMakeFiles/cropIms.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cropIms.dir/depend

