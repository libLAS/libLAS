README for SWIG/C# Project
==========================

This directory contains the code needed to create C# bindings for libLAS using
SWIG.  It is very much still *** UNDER DEVELOPMENT *** so proceed with caution.

If questions, contact mpg@flaxen.com.


* The solution file swig.sln has three projects:
  - swig_CPP
    . runs swig on the .i file
	  - creates the .cpp wrapper file
	  - creates the .cs wrapper classes
	. compiles the .cpp wrapper file
	. creates liblas_swig_cpp.dll
  - swig_CS
    . compiled the .cs wrapper classes
	. creates liblas_swig_cs.dll
  - swig_test
    . compiles a simple test app to verify the bindings work
	. creates swig_test.exe


* The two DLLs and the EXE are put into the main liblas bin directory, for ease of use.


* It would be nice if CMake generated the solution and project files for us,
but googling around a bit it appears CMake's support for Swig/C# isn't quite there 
yet.


* Only Visual Studio 2010 is supported.  Four build configurations are possible:
  - x86/Debug
  - x86/Release
  - x64/Debug
  - x64/Release
Of these four, only x64/Debug has actually been tested :-)

There seems to be an issue with running swig-generated code in x86 mode on a
64-bit OS, and the examples that ship with swig bear this out.  I'm not sure
what's up with that.  See this thread:
  http://old.nabble.com/SWIG-examples-will-not-run-on-my-windows-7---64-bits-td29085404.html


* The test app should be run from the csharp-new/liblas-swig-test directory.
