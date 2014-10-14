marpaWrapper
============

0. Generate the Makefile
   ---------------------

On Windows: cmake -G "NMake Makefiles"
On UNIX   : cmake -G "Unix Makefiles"

To eventually install in a directory of your choice, add : -DCMAKE_INSTALL_PREFIX:PATH=/your/install/path
To disable the (expensive) support of TRACE loglevel, add: -DCMAKE_BUILD_TYPE:STRING=Release

1. Build the project
   -----------------

On Windows: nmake
On UNIX   : make

2. Run the test suite
   ------------------

On Windows: build\marpaWrapper_test
On UNIX   : build/marpaWrapper_test

3. Install
   -------

On Windows: nmake install
On UNIX   : make install

9. Clean or distclean
   ------------------

On Windows: nmake clean
On UNIX   : make clean

On Windows: nmake distclean
On UNIX   : make distclean
