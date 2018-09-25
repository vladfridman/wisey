//
//  yzc.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "Compiler.hpp"
#include "YzcArgumentParser.hpp"
#include "Log.hpp"

using namespace std;
using namespace wisey;

/**
 * Main for running the wisey compiler
 */
int main(int argc, char **argv) {
  vector<string> arguments;
  for (int i = 1; i < argc; i++) {
    arguments.push_back(argv[i]);
  }
  YzcArgumentParser argumentParser;
  CompilerArguments compilerArguments = argumentParser.parse(arguments);
  
  Log::setLogLevel(compilerArguments.isVerbouse() ? INFOLEVEL : ERRORLEVEL);
  
  if (compilerArguments.shouldOutput() && !compilerArguments.getOutputFile().size()) {
    compilerArguments.setOutputFile("runnable.o");
  }
  
  Compiler compiler(compilerArguments);
  compiler.compile();
  if (compilerArguments.shouldOptimize()) {
    compiler.optimize();
  }
  if (compilerArguments.shouldPrintAssembly()) {
    compiler.printAssembly();
  }
  if (compilerArguments.getOutputFile().size()) {
    compiler.saveBinary(compilerArguments.getOutputFile());
  }
  
  return 0;
}
