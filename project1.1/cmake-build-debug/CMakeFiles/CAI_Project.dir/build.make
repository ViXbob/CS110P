# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

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

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\ViXbob\CLionProjects\CAI_Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\CAI_Project.dir\depend.make
# Include the progress variables for this target.
include CMakeFiles\CAI_Project.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\CAI_Project.dir\flags.make

CMakeFiles\CAI_Project.dir\main.c.obj: CMakeFiles\CAI_Project.dir\flags.make
CMakeFiles\CAI_Project.dir\main.c.obj: ..\main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CAI_Project.dir/main.c.obj"
	C:\PROGRA~2\MIB055~1\2019\BUILDT~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\CAI_Project.dir\main.c.obj /FdCMakeFiles\CAI_Project.dir\ /FS -c C:\Users\ViXbob\CLionProjects\CAI_Project\main.c
<<

CMakeFiles\CAI_Project.dir\main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CAI_Project.dir/main.c.i"
	C:\PROGRA~2\MIB055~1\2019\BUILDT~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe > CMakeFiles\CAI_Project.dir\main.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\ViXbob\CLionProjects\CAI_Project\main.c
<<

CMakeFiles\CAI_Project.dir\main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CAI_Project.dir/main.c.s"
	C:\PROGRA~2\MIB055~1\2019\BUILDT~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\CAI_Project.dir\main.c.s /c C:\Users\ViXbob\CLionProjects\CAI_Project\main.c
<<

# Object files for target CAI_Project
CAI_Project_OBJECTS = \
"CMakeFiles\CAI_Project.dir\main.c.obj"

# External object files for target CAI_Project
CAI_Project_EXTERNAL_OBJECTS =

CAI_Project.exe: CMakeFiles\CAI_Project.dir\main.c.obj
CAI_Project.exe: CMakeFiles\CAI_Project.dir\build.make
CAI_Project.exe: CMakeFiles\CAI_Project.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable CAI_Project.exe"
	"C:\Program Files\JetBrains\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\CAI_Project.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\mt.exe --manifests -- C:\PROGRA~2\MIB055~1\2019\BUILDT~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\CAI_Project.dir\objects1.rsp @<<
 /out:CAI_Project.exe /implib:CAI_Project.lib /pdb:C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug\CAI_Project.pdb /version:0.0 /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\CAI_Project.dir\build: CAI_Project.exe
.PHONY : CMakeFiles\CAI_Project.dir\build

CMakeFiles\CAI_Project.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\CAI_Project.dir\cmake_clean.cmake
.PHONY : CMakeFiles\CAI_Project.dir\clean

CMakeFiles\CAI_Project.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\Users\ViXbob\CLionProjects\CAI_Project C:\Users\ViXbob\CLionProjects\CAI_Project C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug C:\Users\ViXbob\CLionProjects\CAI_Project\cmake-build-debug\CMakeFiles\CAI_Project.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\CAI_Project.dir\depend

