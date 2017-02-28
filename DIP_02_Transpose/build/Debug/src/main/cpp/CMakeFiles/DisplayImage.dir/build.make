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
CMAKE_SOURCE_DIR = /home/koldar/git/digital-image-processing-course/DIP_02_Transpose

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug

# Include any dependencies generated for this target.
include src/main/cpp/CMakeFiles/DisplayImage.dir/depend.make

# Include the progress variables for this target.
include src/main/cpp/CMakeFiles/DisplayImage.dir/progress.make

# Include the compile flags for this target's objects.
include src/main/cpp/CMakeFiles/DisplayImage.dir/flags.make

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o: src/main/cpp/CMakeFiles/DisplayImage.dir/flags.make
src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o: ../../src/main/cpp/DisplayImage.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o"
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o -c /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/src/main/cpp/DisplayImage.cpp

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DisplayImage.dir/DisplayImage.cpp.i"
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/src/main/cpp/DisplayImage.cpp > CMakeFiles/DisplayImage.dir/DisplayImage.cpp.i

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DisplayImage.dir/DisplayImage.cpp.s"
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/src/main/cpp/DisplayImage.cpp -o CMakeFiles/DisplayImage.dir/DisplayImage.cpp.s

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.requires:

.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.requires

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.provides: src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.requires
	$(MAKE) -f src/main/cpp/CMakeFiles/DisplayImage.dir/build.make src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.provides.build
.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.provides

src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.provides.build: src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o


# Object files for target DisplayImage
DisplayImage_OBJECTS = \
"CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o"

# External object files for target DisplayImage
DisplayImage_EXTERNAL_OBJECTS =

DisplayImage: src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o
DisplayImage: src/main/cpp/CMakeFiles/DisplayImage.dir/build.make
DisplayImage: /usr/local/lib/libopencv_shape.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_stitching.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_superres.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_videostab.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_objdetect.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_calib3d.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_features2d.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_flann.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_highgui.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_ml.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_photo.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_video.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_videoio.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_imgcodecs.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_imgproc.so.3.2.0
DisplayImage: /usr/local/lib/libopencv_core.so.3.2.0
DisplayImage: src/main/cpp/CMakeFiles/DisplayImage.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../DisplayImage"
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/DisplayImage.dir/link.txt --verbose=$(VERBOSE)
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && /usr/bin/cmake -E copy_directory /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/../../src/main/resources /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug

# Rule to build all files generated by this target.
src/main/cpp/CMakeFiles/DisplayImage.dir/build: DisplayImage

.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/build

src/main/cpp/CMakeFiles/DisplayImage.dir/requires: src/main/cpp/CMakeFiles/DisplayImage.dir/DisplayImage.cpp.o.requires

.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/requires

src/main/cpp/CMakeFiles/DisplayImage.dir/clean:
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp && $(CMAKE_COMMAND) -P CMakeFiles/DisplayImage.dir/cmake_clean.cmake
.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/clean

src/main/cpp/CMakeFiles/DisplayImage.dir/depend:
	cd /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/koldar/git/digital-image-processing-course/DIP_02_Transpose /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/src/main/cpp /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp /home/koldar/git/digital-image-processing-course/DIP_02_Transpose/build/Debug/src/main/cpp/CMakeFiles/DisplayImage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/main/cpp/CMakeFiles/DisplayImage.dir/depend

