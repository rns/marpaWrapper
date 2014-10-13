marpaWrapper
============

libmarpa wrapper

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

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/lib make
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/lib make test
