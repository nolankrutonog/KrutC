# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/nolankrutonog/Desktop/workspace/KrutC/krutc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build

# Include any dependencies generated for this target.
include CMakeFiles/krutc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/krutc.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/krutc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/krutc.dir/flags.make

CMakeFiles/krutc.dir/src/main.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/main.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/main.cpp
CMakeFiles/krutc.dir/src/main.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/krutc.dir/src/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/main.cpp.o -MF CMakeFiles/krutc.dir/src/main.cpp.o.d -o CMakeFiles/krutc.dir/src/main.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/main.cpp

CMakeFiles/krutc.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/main.cpp > CMakeFiles/krutc.dir/src/main.cpp.i

CMakeFiles/krutc.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/main.cpp -o CMakeFiles/krutc.dir/src/main.cpp.s

CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/lexer.cpp
CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o -MF CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o.d -o CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/lexer.cpp

CMakeFiles/krutc.dir/src/frontend/lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/frontend/lexer.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/lexer.cpp > CMakeFiles/krutc.dir/src/frontend/lexer.cpp.i

CMakeFiles/krutc.dir/src/frontend/lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/frontend/lexer.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/lexer.cpp -o CMakeFiles/krutc.dir/src/frontend/lexer.cpp.s

CMakeFiles/krutc.dir/src/frontend/parser.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/frontend/parser.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/parser.cpp
CMakeFiles/krutc.dir/src/frontend/parser.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/krutc.dir/src/frontend/parser.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/frontend/parser.cpp.o -MF CMakeFiles/krutc.dir/src/frontend/parser.cpp.o.d -o CMakeFiles/krutc.dir/src/frontend/parser.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/parser.cpp

CMakeFiles/krutc.dir/src/frontend/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/frontend/parser.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/parser.cpp > CMakeFiles/krutc.dir/src/frontend/parser.cpp.i

CMakeFiles/krutc.dir/src/frontend/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/frontend/parser.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/parser.cpp -o CMakeFiles/krutc.dir/src/frontend/parser.cpp.s

CMakeFiles/krutc.dir/src/frontend/tree.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/frontend/tree.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/tree.cpp
CMakeFiles/krutc.dir/src/frontend/tree.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/krutc.dir/src/frontend/tree.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/frontend/tree.cpp.o -MF CMakeFiles/krutc.dir/src/frontend/tree.cpp.o.d -o CMakeFiles/krutc.dir/src/frontend/tree.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/tree.cpp

CMakeFiles/krutc.dir/src/frontend/tree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/frontend/tree.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/tree.cpp > CMakeFiles/krutc.dir/src/frontend/tree.cpp.i

CMakeFiles/krutc.dir/src/frontend/tree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/frontend/tree.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/tree.cpp -o CMakeFiles/krutc.dir/src/frontend/tree.cpp.s

CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/typechecker.cpp
CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o -MF CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o.d -o CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/typechecker.cpp

CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/typechecker.cpp > CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.i

CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/typechecker.cpp -o CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.s

CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/scopetable.cpp
CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o -MF CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o.d -o CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/scopetable.cpp

CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/scopetable.cpp > CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.i

CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/frontend/scopetable.cpp -o CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.s

CMakeFiles/krutc.dir/src/backend/codegen.cpp.o: CMakeFiles/krutc.dir/flags.make
CMakeFiles/krutc.dir/src/backend/codegen.cpp.o: /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/backend/codegen.cpp
CMakeFiles/krutc.dir/src/backend/codegen.cpp.o: CMakeFiles/krutc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/krutc.dir/src/backend/codegen.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/krutc.dir/src/backend/codegen.cpp.o -MF CMakeFiles/krutc.dir/src/backend/codegen.cpp.o.d -o CMakeFiles/krutc.dir/src/backend/codegen.cpp.o -c /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/backend/codegen.cpp

CMakeFiles/krutc.dir/src/backend/codegen.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/krutc.dir/src/backend/codegen.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/backend/codegen.cpp > CMakeFiles/krutc.dir/src/backend/codegen.cpp.i

CMakeFiles/krutc.dir/src/backend/codegen.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/krutc.dir/src/backend/codegen.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/src/backend/codegen.cpp -o CMakeFiles/krutc.dir/src/backend/codegen.cpp.s

# Object files for target krutc
krutc_OBJECTS = \
"CMakeFiles/krutc.dir/src/main.cpp.o" \
"CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o" \
"CMakeFiles/krutc.dir/src/frontend/parser.cpp.o" \
"CMakeFiles/krutc.dir/src/frontend/tree.cpp.o" \
"CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o" \
"CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o" \
"CMakeFiles/krutc.dir/src/backend/codegen.cpp.o"

# External object files for target krutc
krutc_EXTERNAL_OBJECTS =

krutc: CMakeFiles/krutc.dir/src/main.cpp.o
krutc: CMakeFiles/krutc.dir/src/frontend/lexer.cpp.o
krutc: CMakeFiles/krutc.dir/src/frontend/parser.cpp.o
krutc: CMakeFiles/krutc.dir/src/frontend/tree.cpp.o
krutc: CMakeFiles/krutc.dir/src/frontend/typechecker.cpp.o
krutc: CMakeFiles/krutc.dir/src/frontend/scopetable.cpp.o
krutc: CMakeFiles/krutc.dir/src/backend/codegen.cpp.o
krutc: CMakeFiles/krutc.dir/build.make
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMMCJIT.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMOrcJIT.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64CodeGen.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64AsmParser.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64Disassembler.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMExecutionEngine.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMRuntimeDyld.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMPasses.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMCoroutines.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMHipStdPar.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMipo.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMFrontendOpenMP.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMFrontendOffloading.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMLinker.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMIRPrinter.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMVectorize.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMInstrumentation.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMJITLink.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMOrcTargetProcess.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMOrcShared.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMWindowsDriver.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMOption.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAsmPrinter.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMCFGuard.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMGlobalISel.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMSelectionDAG.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMCodeGen.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMTarget.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMBitWriter.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMScalarOpts.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAggressiveInstCombine.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMInstCombine.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMObjCARCOpts.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMTransformUtils.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAnalysis.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMProfileData.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMSymbolize.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDebugInfoDWARF.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDebugInfoPDB.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMObject.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMIRReader.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAsmParser.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMBitReader.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMTextAPI.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDebugInfoMSF.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDebugInfoBTF.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMMCParser.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64Desc.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMCodeGenTypes.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64Info.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMAArch64Utils.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMCore.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMRemarks.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMBitstreamReader.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMMCDisassembler.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMMC.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMBinaryFormat.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMTargetParser.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDebugInfoCodeView.a
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMSupport.a
krutc: /opt/homebrew/lib/libz3.dylib
krutc: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.0.sdk/usr/lib/libz.tbd
krutc: /opt/homebrew/lib/libzstd.dylib
krutc: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.0.sdk/usr/lib/libcurses.tbd
krutc: /opt/homebrew/Cellar/llvm/18.1.5/lib/libLLVMDemangle.a
krutc: CMakeFiles/krutc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable krutc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/krutc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/krutc.dir/build: krutc
.PHONY : CMakeFiles/krutc.dir/build

CMakeFiles/krutc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/krutc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/krutc.dir/clean

CMakeFiles/krutc.dir/depend:
	cd /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nolankrutonog/Desktop/workspace/KrutC/krutc /Users/nolankrutonog/Desktop/workspace/KrutC/krutc /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build /Users/nolankrutonog/Desktop/workspace/KrutC/krutc/build/CMakeFiles/krutc.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/krutc.dir/depend

