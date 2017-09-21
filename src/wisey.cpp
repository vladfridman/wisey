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

  Compiler compiler(compilerArguents);
  compiler.compile();
  if (!compilerArguents.getOutputFile().size()) {
    compiler.run();
  }
  
  return 0;
}
