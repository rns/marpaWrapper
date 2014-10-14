marpaWrapper
============

0. Generate the Makefile
   ---------------------

On Windows: cmake -G "NMake Makefiles"
On UNIX   : cmake -G "Unix Makefiles"

1. Build the project
   -----------------

On Windows: nmake
On UNIX   : make

2. Run the test suite
   ------------------

On Windows: build\marpaWrapper_test
On UNIX   : build/marpaWrapper_test

9. Clean or distclean
   ------------------

On Windows: nmake clean
On UNIX   : make clean

On Windows: nmake distclean
On UNIX   : make distclean
