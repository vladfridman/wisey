//
//  wiseyc.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Compiler.hpp"
#include "wisey/CompilerArguments.hpp"
#include "wisey/CompilerArgumentParser.hpp"
#include "wisey/Log.hpp"

using namespace wisey;

/**
 * Main for running the wisey compiler
 */
int main(int argc, char **argv) {
  Log::setLogLevel(ERRORLEVEL);
  
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguents = compilerArgumentParser.parse(argc, argv);
  
  Compiler compiler(compilerArguents);
  compiler.compile();
  if (!compilerArguents.getOutputFile().size()) {
    compiler.run();
  }

  return 0;
}
