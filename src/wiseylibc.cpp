//
//  wiseylibc.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "CompilerDriver.hpp"

using namespace std;
using namespace wisey;

/**
 * Main for running the wiseylibc compiler to compile a wisey library
 */
int main(int argc, char **argv) {
  CompilerDriver compilerDriver;
  compilerDriver.compileLibrary(argc, argv);
  
  return 0;
}
