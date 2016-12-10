## Yazyk Language Compiler ##

### Prerequisites ###

#### XCode ####

Install XCode and clang version 4.0.0 or later

#### LLVM libraries and scripts ####

You need LLVM version 4.0.0svn installed in /usr/local/include and /usr/local/lib
To install LLVM follow instructions [here](http://llvm.org/docs/GettingStarted.html).
Run 'make install' after cmake.

### Quick Start ###

To compile type:

    make

This generates a binary for the compiler bin/yazyk

To compile a yazyk sample file type:

    bin/yazyk samples/input.yz

