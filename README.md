## Wisey Language Compiler ##

### Developer set up on Mac OS X x86 64 bit ###

Prerequisites: g++ installed

1. Install Xcode and command line tools: `xcode-select --install`
2. Update apt-get: `sudo apt-get update`
3. Install prerequesites: `sudo apt-get install cmake subversion python flex bison`
4. Install LLVM
  `cd ~; mkdir llvm; cd llvm;`
  `svn co http://llvm.org/svn/llvm-project/llvm/branches/release_70 llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/branches/release_70 clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild;`
  `cmake -G Unix\ Makefiles ~llvm/llvm;`
  `make -j8;`
  `sudo make install;`
5. Otionally make Xcode project for LLVM for reference to LLVM usage
  `mkdir ~/llvm/llvmxcodebuild; cd ~/llvm/llvmxcodebuild;`
  `cmake -G Xcode CMAKE_BUILD_TYPE="Debug" /Users/vlad/llvm/llvm;`
  `open LLVM.xcodeproj;`
  Create a scheme only for clang
6. Checkout wisey code
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~; git clone git@github.com:vladfridman/wisey.git;`
7. Compile wisey compiler
  `cd ~/wisey; make -j8 bin/yzc bin/wiseyc bin/wiseylibc;`
8. Install wisey compiler
  `mkdir ~/wiseyhome; mkdir ~/wiseyhome/bin; mkdir ~/wiseyhome/lib; mkdir ~/wiseyhome/headers; export WISEY_HOME=~/wiseyhome; export PATH=$PATH:$WISEY_HOME/bin;`
  `cp bin/* ~/wiseyhome/bin;`
10. Compile wisey library
  `cd ~/wisey;`
  `g++ -c -pipe -O3 -fomit-frame-pointer -march=native -std=c++11 libsrc/libdata/libdata.cpp -o lib/libdata.o;` 
  `bin/wiseylibc -v -Alib/libdata.o wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -o lib/libwisey.a -H wisey/headers/libwisey.yz && rm lib/libwisey.o lib/libdata.o;`
  `cp lib/libwisey.a $WISEY_HOME/lib; cp wisey/headers/libwisey.yz $WISEY_HOME/headers;`
11. Install googletest and googlemock:
  `cd ~; git clone https://github.com/google/googletest.git; cd ~/googletest/googletest/make;`
  modify Makefile and add -fvisibility-inlines-hidden to CXXFLAGS
  `make; ar -rv libgtest.a gtest-all.o;`
  `sudo cp libgtest.a /usr/local/lib/libgtest.a;` 
  `sudo cp -r ../include/gtest /usr/local/include/;`
  `cd ~/googletest/googlemock/make;`
  modify Makefile and add -fvisibility-inlines-hidden to CXXFLAGS
  `make; ar -rv libgmock.a gmock-all.o;`
  `sudo cp libgmock.a /usr/local/lib/libgmock.a;`
  `sudo cp -r ../include/gmock /usr/local/include/;`
12. Make tests
  `cd ~/wisey; make -j8 tests;`
  `bin/runtests`

### Developer set up on Ubuntu Linux x86 64 bit ###

1. Update apt-get: `sudo apt-get update`
2. Install prerequesites: `sudo apt-get install cmake subversion python g++ flex bison`
3. Install LLVM 
  `cd ~; mkdir llvm; cd llvm; svn co http://llvm.org/svn/llvm-project/llvm/branches/release_70 llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/branches/release_70 clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild; cmake -G Unix\ Makefiles ~/llvm/llvm;`
  `make -j8;`
  `sudo make install;`
4. Checkout wisey code: 
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~; git clone git@github.com:vladfridman/wisey.git`
5. Compile wisey compiler
  `cd ~/wisey; make -j8 bin/yzc bin/wiseyc bin/wiseylibc;`
6. Install wisey compiler
  `mkdir ~/wiseyhome; mkdir ~/wiseyhome/bin; mkdir ~/wiseyhome/lib; mkdir ~/wiseyhome/headers; export WISEY_HOME=~/wiseyhome; export PATH=$PATH:$WISEY_HOME/bin;`
  `cp bin/* ~/wiseyhome/bin;`
7. Compile wisey library
  `cd ~/wisey;`
  `g++ -c -pipe -O3 -fomit-frame-pointer -march=native -std=c++11 libsrc/libdata/libdata.cpp -o lib/libdata.o;` 
  `bin/wiseylibc -v -Alib/libdata.o wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -o lib/libwisey.a -H wisey/headers/libwisey.yz && rm lib/libwisey.o lib/libdata.o;`
  `cp lib/libwisey.a $WISEY_HOME/lib; cp wisey/headers/libwisey.yz $WISEY_HOME/headers;`
8. Install googletest and googlemock:
  `cd ~; git clone https://github.com/google/googletest.git; cd ~/googletest/googletest/make;`
  `make; ar -rv libgtest.a gtest-all.o;`
  `sudo cp libgtest.a /usr/local/lib/libgtest.a;` 
  `sudo cp -r ../include/gtest /usr/local/include/;`
  `cd ~/googletest/googlemock/make; `
  `make; ar -rv libgmock.a gmock-all.o;`
  `sudo cp libgmock.a /usr/local/lib/libgmock.a;`
  `sudo cp -r ../include/gmock /usr/local/include/;`
9. Make tests
  `cd ~/wisey; make -j8 tests;`
  `bin/runtests`

