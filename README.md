## Wisey Language Compiler ##

### Developer set up instructions ###

Prerequisites: Mac OS X 64 bit

1. Install Xcode and command line tools: `xcode-select --install`
2. Install CMake from cmake.org; `sudo ln -s /Applications/CMake.app/Contents/bin/cmake /usr/local/bin/cmake`
3. Install LLVM
  `cd ~; mkdir llvm; cd llvm;`
  `svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm;`
  `cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/trunk clang;`
  `mkdir ~/llvm/llvmbuild; cd ~/llvm/llvmbuild;`
  `cmake -G Unix\ Makefiles /Users/vlad/llvm/llvm;`
  `make -j8;`
  `sudo make install;`
4. Otionally make Xcode project for LLVM for reference to LLVM usage
  `mkdir ~/llvm/llvmxcodebuild; cd ~/llvm/llvmxcodebuild;`
  `cmake -G Xcode CMAKE_BUILD_TYPE="Debug" /Users/vlad/llvm/llvm;`
  `open LLVM.xcodeproj;`
  Create a scheme only for clang
5. Install homebrew
6. Install bison
  `brew install bison;`
  `brew link bison --force;`
  `source ~/.profile;`
7. Checkout wisey code
  Setup SSH https://help.github.com/articles/generating-an-ssh-key/
  `cd ~;`
  `git clone git@github.com:vladfridman/wisey.git;`
8. Compile wisey compiler
  `cd ~/wisey;`
  `make -j8; make -j8 tests;`
9. Compile wisey library
  `cd ~/wisey;`
  `bin/wiseyc wisey/lang/*.yz wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz;`
  `g++ -shared -o /Users/vlad/wisey/lib/libwisey.so lib/libdata.a compiled.o;`
  `bin/wiseyc wisey/lang/*.yz  wisey/threads/*.yz wisey/io/*.yz wisey/data/*.yz -H /Users/vlad/wisey/wisey/headers/libwisey.yz;`
  `rm ./compiled.o;`
10. Test installation correctness:
  `cd ~/wisey;`
  `bin/runtests;`
