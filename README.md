## Wisey Language Compiler ##

### Developer set up instructions ###

Prerequisites: Mac OS X 64 bit

1. Install Xcode and command line tools: `xcode-select --install`
2. Update apt-get: sudo apt-get update
3. Install CMake from cmake.org: sudo apt-get install cmake
4. Install LLVM
  `cd ~; mkdir llvm; cd llvm;`
  `svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/trunk clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild;`
  `cmake -G Unix\ Makefiles ~llvm/llvm;`
  `make -j8;`
  `sudo make install;`
5. Otionally make Xcode project for LLVM for reference to LLVM usage
  `mkdir ~/llvm/llvmxcodebuild; cd ~/llvm/llvmxcodebuild;`
  `cmake -G Xcode CMAKE_BUILD_TYPE="Debug" /Users/vlad/llvm/llvm;`
  `open LLVM.xcodeproj;`
  Create a scheme only for clang
6. Install homebrew
7. Install bison
  `brew install bison;`
  `brew link bison --force;`
  `source ~/.profile;`
8. Checkout wisey code
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~;`
  `git clone git@github.com:vladfridman/wisey.git;`
9. Compile wisey compiler
  `cd ~/wisey;`
  `make -j8; make -j8 tests;`
10. Compile wisey library
  `cd ~/wisey;`
  `bin/wiseyc wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz;`
  `g++ -shared -o /Users/vlad/wisey/lib/libwisey.so lib/libdata.a compiled.o;`
  `bin/wiseyc wisey/lang/*.yz  wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -H /Users/vlad/wisey/wisey/headers/libwisey.yz;`
  `rm ./compiled.o;`
11. Test installation correctness:
  `cd ~/wisey;`
  `bin/runtests;`
