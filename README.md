## Wisey Language Compiler ##

### Download Wisey compiler ###

http://wisey.ninja/download.html

### Read language reference ###

http://wisey.ninja/language.html

### Explore  wisey library ###

http:://wisey.ninja

### Developer set up on Mac OS X x86 64 bit ###

Prerequisites: g++ installed

1. Install Xcode from App Store, Install command line tools: `xcode-select --install`
2. Install prerequesites: `sudo apt-get update; sudo apt-get install cmake subversion python flex bison`
3. Install LLVM
  `cd ~; mkdir llvm; cd llvm;`
  `svn co http://llvm.org/svn/llvm-project/llvm/branches/release_70 llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/branches/release_70 clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild;`
  `cmake -G Unix\ Makefiles ~llvm/llvm;`
  `make -j8;`
  `sudo make install;`
4. Otionally make Xcode project for LLVM for reference to LLVM usage
  `mkdir ~/llvm/llvmxcodebuild; cd ~/llvm/llvmxcodebuild;`
  `cmake -G Xcode CMAKE_BUILD_TYPE="Debug" /Users/vlad/llvm/llvm;`
  `open LLVM.xcodeproj;`
  Create a scheme only for clang
5. Checkout wisey code
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~; git clone git@github.com:vladfridman/wiseydev.git;`
6. Compile wisey compiler
  `cd ~/wiseydev; make -j8 bin/yzc bin/wiseyc bin/wiseylibc;`
7. Install wisey compiler
  `mkdir ~/wisey; mkdir ~/wisey/bin; mkdir ~/wisey/lib; mkdir ~/wisey/headers; export WISEY_HOME=~/wisey; export PATH=$PATH:$WISEY_HOME/bin;`
  `cp bin/* ~/wisey/bin;`
8. Compile wisey library
  `cd ~/wiseydev;`
  `g++ -c -pipe -O3 -fomit-frame-pointer -march=native -std=c++11 libsrc/libdata/libdata.cpp -o build/libdata.o;` 
  `bin/wiseylibc -v -Abuild/libdata.o wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -o build/libwisey.a -H build/libwisey.yz`
  `cp build/libwisey.a $WISEY_HOME/lib; cp build/libwisey.yz $WISEY_HOME/headers;`
9. Install googletest and googlemock:
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
10. Make tests
  `cd ~/wisey; make -j8 tests;`
  `bin/runtests`

### Developer set up on Ubuntu Linux x86 64 bit ###

1. Install prerequesites: `sudo apt-get update; sudo apt-get install cmake subversion python g++ flex bison`
2. Install LLVM 
  `cd ~; mkdir llvm; cd llvm; svn co http://llvm.org/svn/llvm-project/llvm/branches/release_70 llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/branches/release_70 clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild; cmake -G Unix\ Makefiles ~/llvm/llvm;`
  `make -j8;`
  `sudo make install;`
3. Checkout wisey code: 
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~; git clone git@github.com:vladfridman/wiseydev.git`
4. Compile wisey compiler
  `cd ~/wiseydev; make -j8 bin/yzc bin/wiseyc bin/wiseylibc;`
5. Install wisey compiler
  `mkdir ~/wisey; mkdir ~/wisey/bin; mkdir ~/wisey/lib; mkdir ~/wisey/headers; export WISEY_HOME=~/wisey; export PATH=$PATH:$WISEY_HOME/bin;`
  `cp bin/* ~/wisey/bin;`
6. Compile wisey library
  `cd ~/wisey;`
  `g++ -c -pipe -O3 -fomit-frame-pointer -march=native -std=c++11 libsrc/libdata/libdata.cpp -o build/libdata.o;` 
  `bin/wiseylibc -v -Alib/libdata.o wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -o build/libwisey.a -H build/libwisey.yz && rm lib/libwisey.o lib/libdata.o;`
  `cp build/libwisey.a $WISEY_HOME/lib; cp build/libwisey.yz $WISEY_HOME/headers;`
7. Install googletest and googlemock:
  `cd ~; git clone https://github.com/google/googletest.git; cd ~/googletest/googletest/make;`
  `make; ar -rv libgtest.a gtest-all.o;`
  `sudo cp libgtest.a /usr/local/lib/libgtest.a;` 
  `sudo cp -r ../include/gtest /usr/local/include/;`
  `cd ~/googletest/googlemock/make; `
  `make; ar -rv libgmock.a gmock-all.o;`
  `sudo cp libgmock.a /usr/local/lib/libgmock.a;`
  `sudo cp -r ../include/gmock /usr/local/include/;`
8. Make tests
  `cd ~/wisey; make -j8 tests;`
  `bin/runtests` # tests that run wisey files do not work on Linux

