marpaWrapper
============

0. First, compile and install libmarpa.
   ------------------------------------

For example, git clone https://github.com/jeffreykegler/libmarpa, and the,

make dist
cd dist
./configure --enable-shared --disable-static --prefix=/tmp
make
make install

1. Second, build marpaWrapper
   --------------------------

Usage: On Windows: cmake -G "NMake Makefiles", then nmake test
       On UNIX   : cmake -G "UNIX Makefiles", then make test

If marpa is in a non-standard path, e.g. /tmp:

/tmp/lib/libmarpa.so
/tmp/lib/libmarpa-7.2.0.so
/tmp/lib/libmarpa.la
/tmp/include
/tmp/include/marpa_ami.h
/tmp/include/marpa.h
/tmp/include/marpa_avl.h
/tmp/include/marpa_tavl.h
/tmp/include/marpa_slif.h
/tmp/include/marpa_obs.h

play with $LIB_LIBRARY_PATH or %PATH%, i.e.:

cmake -G "Unix Makefiles" -DLIBMARPA_ROOT_DIR=/tmp
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/lib make
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/lib build/marpaWrapper_test
