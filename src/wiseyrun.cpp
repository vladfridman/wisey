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

using namespace std;
using namespace wisey;

/**
 * Main for running the wisey compiler driver
 */
int main(int argc, char** argv) {
  Log::setLogLevel(DEBUGLEVEL);
  
  vector<string> arguments;
  for (int i = 1; i < argc; i++) {
    arguments.push_back(argv[i]);
  }
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguments = compilerArgumentParser.parse(arguments);

  Compiler compiler(compilerArguments);
  compiler.compile();
  if (!compilerArguments.getOutputFile().size()) {
    compiler.run(argc, argv);
  }
  
  return 0;
}
