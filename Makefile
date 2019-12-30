# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /home/titus/clion-2018.3.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/titus/clion-2018.3.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/titus/programming/c++/vkEngine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/titus/programming/c++/vkEngine

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/home/titus/clion-2018.3.2/bin/cmake/linux/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/home/titus/clion-2018.3.2/bin/cmake/linux/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/titus/programming/c++/vkEngine/CMakeFiles /home/titus/programming/c++/vkEngine/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/titus/programming/c++/vkEngine/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named vkEngine

# Build rule for target.
vkEngine: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 vkEngine
.PHONY : vkEngine

# fast build rule for target.
vkEngine/fast:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/build
.PHONY : vkEngine/fast

main.o: main.cpp.o

.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/main.cpp.s
.PHONY : main.cpp.s

vkRenderer/MoeVkInstance.o: vkRenderer/MoeVkInstance.cpp.o

.PHONY : vkRenderer/MoeVkInstance.o

# target to build an object file
vkRenderer/MoeVkInstance.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkInstance.cpp.o
.PHONY : vkRenderer/MoeVkInstance.cpp.o

vkRenderer/MoeVkInstance.i: vkRenderer/MoeVkInstance.cpp.i

.PHONY : vkRenderer/MoeVkInstance.i

# target to preprocess a source file
vkRenderer/MoeVkInstance.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkInstance.cpp.i
.PHONY : vkRenderer/MoeVkInstance.cpp.i

vkRenderer/MoeVkInstance.s: vkRenderer/MoeVkInstance.cpp.s

.PHONY : vkRenderer/MoeVkInstance.s

# target to generate assembly for a file
vkRenderer/MoeVkInstance.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkInstance.cpp.s
.PHONY : vkRenderer/MoeVkInstance.cpp.s

vkRenderer/MoeVkLogicalDevice.o: vkRenderer/MoeVkLogicalDevice.cpp.o

.PHONY : vkRenderer/MoeVkLogicalDevice.o

# target to build an object file
vkRenderer/MoeVkLogicalDevice.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkLogicalDevice.cpp.o
.PHONY : vkRenderer/MoeVkLogicalDevice.cpp.o

vkRenderer/MoeVkLogicalDevice.i: vkRenderer/MoeVkLogicalDevice.cpp.i

.PHONY : vkRenderer/MoeVkLogicalDevice.i

# target to preprocess a source file
vkRenderer/MoeVkLogicalDevice.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkLogicalDevice.cpp.i
.PHONY : vkRenderer/MoeVkLogicalDevice.cpp.i

vkRenderer/MoeVkLogicalDevice.s: vkRenderer/MoeVkLogicalDevice.cpp.s

.PHONY : vkRenderer/MoeVkLogicalDevice.s

# target to generate assembly for a file
vkRenderer/MoeVkLogicalDevice.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkLogicalDevice.cpp.s
.PHONY : vkRenderer/MoeVkLogicalDevice.cpp.s

vkRenderer/MoeVkPhysicalDevice.o: vkRenderer/MoeVkPhysicalDevice.cpp.o

.PHONY : vkRenderer/MoeVkPhysicalDevice.o

# target to build an object file
vkRenderer/MoeVkPhysicalDevice.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkPhysicalDevice.cpp.o
.PHONY : vkRenderer/MoeVkPhysicalDevice.cpp.o

vkRenderer/MoeVkPhysicalDevice.i: vkRenderer/MoeVkPhysicalDevice.cpp.i

.PHONY : vkRenderer/MoeVkPhysicalDevice.i

# target to preprocess a source file
vkRenderer/MoeVkPhysicalDevice.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkPhysicalDevice.cpp.i
.PHONY : vkRenderer/MoeVkPhysicalDevice.cpp.i

vkRenderer/MoeVkPhysicalDevice.s: vkRenderer/MoeVkPhysicalDevice.cpp.s

.PHONY : vkRenderer/MoeVkPhysicalDevice.s

# target to generate assembly for a file
vkRenderer/MoeVkPhysicalDevice.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkPhysicalDevice.cpp.s
.PHONY : vkRenderer/MoeVkPhysicalDevice.cpp.s

vkRenderer/MoeVkRenderer.o: vkRenderer/MoeVkRenderer.cpp.o

.PHONY : vkRenderer/MoeVkRenderer.o

# target to build an object file
vkRenderer/MoeVkRenderer.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkRenderer.cpp.o
.PHONY : vkRenderer/MoeVkRenderer.cpp.o

vkRenderer/MoeVkRenderer.i: vkRenderer/MoeVkRenderer.cpp.i

.PHONY : vkRenderer/MoeVkRenderer.i

# target to preprocess a source file
vkRenderer/MoeVkRenderer.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkRenderer.cpp.i
.PHONY : vkRenderer/MoeVkRenderer.cpp.i

vkRenderer/MoeVkRenderer.s: vkRenderer/MoeVkRenderer.cpp.s

.PHONY : vkRenderer/MoeVkRenderer.s

# target to generate assembly for a file
vkRenderer/MoeVkRenderer.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkRenderer.cpp.s
.PHONY : vkRenderer/MoeVkRenderer.cpp.s

vkRenderer/MoeVkSwapChain.o: vkRenderer/MoeVkSwapChain.cpp.o

.PHONY : vkRenderer/MoeVkSwapChain.o

# target to build an object file
vkRenderer/MoeVkSwapChain.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkSwapChain.cpp.o
.PHONY : vkRenderer/MoeVkSwapChain.cpp.o

vkRenderer/MoeVkSwapChain.i: vkRenderer/MoeVkSwapChain.cpp.i

.PHONY : vkRenderer/MoeVkSwapChain.i

# target to preprocess a source file
vkRenderer/MoeVkSwapChain.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkSwapChain.cpp.i
.PHONY : vkRenderer/MoeVkSwapChain.cpp.i

vkRenderer/MoeVkSwapChain.s: vkRenderer/MoeVkSwapChain.cpp.s

.PHONY : vkRenderer/MoeVkSwapChain.s

# target to generate assembly for a file
vkRenderer/MoeVkSwapChain.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/MoeVkSwapChain.cpp.s
.PHONY : vkRenderer/MoeVkSwapChain.cpp.s

vkRenderer/VkWindow.o: vkRenderer/VkWindow.cpp.o

.PHONY : vkRenderer/VkWindow.o

# target to build an object file
vkRenderer/VkWindow.cpp.o:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/VkWindow.cpp.o
.PHONY : vkRenderer/VkWindow.cpp.o

vkRenderer/VkWindow.i: vkRenderer/VkWindow.cpp.i

.PHONY : vkRenderer/VkWindow.i

# target to preprocess a source file
vkRenderer/VkWindow.cpp.i:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/VkWindow.cpp.i
.PHONY : vkRenderer/VkWindow.cpp.i

vkRenderer/VkWindow.s: vkRenderer/VkWindow.cpp.s

.PHONY : vkRenderer/VkWindow.s

# target to generate assembly for a file
vkRenderer/VkWindow.cpp.s:
	$(MAKE) -f CMakeFiles/vkEngine.dir/build.make CMakeFiles/vkEngine.dir/vkRenderer/VkWindow.cpp.s
.PHONY : vkRenderer/VkWindow.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... vkEngine"
	@echo "... edit_cache"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... vkRenderer/MoeVkInstance.o"
	@echo "... vkRenderer/MoeVkInstance.i"
	@echo "... vkRenderer/MoeVkInstance.s"
	@echo "... vkRenderer/MoeVkLogicalDevice.o"
	@echo "... vkRenderer/MoeVkLogicalDevice.i"
	@echo "... vkRenderer/MoeVkLogicalDevice.s"
	@echo "... vkRenderer/MoeVkPhysicalDevice.o"
	@echo "... vkRenderer/MoeVkPhysicalDevice.i"
	@echo "... vkRenderer/MoeVkPhysicalDevice.s"
	@echo "... vkRenderer/MoeVkRenderer.o"
	@echo "... vkRenderer/MoeVkRenderer.i"
	@echo "... vkRenderer/MoeVkRenderer.s"
	@echo "... vkRenderer/MoeVkSwapChain.o"
	@echo "... vkRenderer/MoeVkSwapChain.i"
	@echo "... vkRenderer/MoeVkSwapChain.s"
	@echo "... vkRenderer/VkWindow.o"
	@echo "... vkRenderer/VkWindow.i"
	@echo "... vkRenderer/VkWindow.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

