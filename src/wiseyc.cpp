//
//  wiseyc.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CompilerDriver.hpp"

using namespace std;
using namespace wisey;

/**
 * Main for running the wisey compiler
 */
int main(int argc, char **argv) {
  CompilerDriver compilerDriver;
  compilerDriver.compileRunnable(argc, argv);
  
  return 0;
}
