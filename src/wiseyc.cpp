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
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguments = compilerArgumentParser.parse(argc, argv);

  Log::setLogLevel(compilerArguments.isVerbouse() ? INFOLEVEL : ERRORLEVEL);
  
  if (compilerArguments.shouldOutput() && !compilerArguments.getOutputFile().size()) {
    compilerArguments.setOutputFile("compiled.o");
  }

  Compiler compiler(compilerArguments);
  compiler.compile();

  return 0;
}
