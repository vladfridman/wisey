//
//  wisey.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Compiler.hpp"
#include "wisey/CompilerArguments.hpp"
#include "wisey/CompilerArgumentParser.hpp"
#include "wisey/Log.hpp"

using namespace wisey;

/**
 * Main for running the wisey compiler driver
 */
int main(int argc, char **argv) {
  Log::setLogLevel(DEBUGLEVEL);
  
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguents = compilerArgumentParser.parse(argc, argv);

  Compiler compiler;
  compiler.compile(compilerArguents.getSourceFiles(), !compilerArguents.getOutputFile().size());

  if (compilerArguents.shouldPrintAssembly()) {
    compiler.printAssembly();
  }
  
  if (compilerArguents.getOutputFile().size()) {
    compiler.saveBitcode(compilerArguents.getOutputFile());
  } else {
    compiler.run();
  }
  
  return 0;
}
