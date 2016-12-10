## Yazyk Language Compiler ##

It is based on LLVM and requires installation of LLVM libraries

### Quick Start ###

To compile type:

    make

This generates a binary for the compiler bin/yazyk

To compile a yazyk sample file type:

    bin/yazyk samples/input.yz

### Prerequisites ###

#### Google Test and Google Mock ####

    cd ~
    git clone https://github.com/google/googletest.git
    cd /googletest/googletest/make
    modify Makefile and add -fvisibility-inlines-hidden to CXXFLAGS
    make
    ar -rv libgtest.a gtest-all.o
    mv libgtest.a /usr/local/lib/libgtest.a 

Similar installation for googlemock

    cd /googletest/googlemock/make
    modify Makefile and add -fvisibility-inlines-hidden to CXXFLAGS
    make
    ar -rv libgmock.a gmock-all.o
    mv libgmock.a /usr/local/lib/libgmock.a 
