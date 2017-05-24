//
//  CompilerArgumentParser.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CompilerArgumentParser.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

void CompilerArgumentParser::printSyntaxAndExit() const {
  Log::e("Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>");
  exit(1);
}

CompilerArguments CompilerArgumentParser::parse(int argc, char **argv) const {
  CompilerArguments compilerArguments;

  if (argc <= 1) {
    printSyntaxAndExit();
  }
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      printSyntaxAndExit();
    }
    if (!strcmp(argv[i], "--emit-llvm")) {
      compilerArguments.setShouldPrintAssembly(true);
      continue;
    }
    if (!strcmp(argv[i], "-o") && i == argc - 1) {
      Log::e("You need to specify the output file name after \"-o\"");
      exit(1);
    }
    if (!strcmp(argv[i], "-o")) {
      i++;
      compilerArguments.setOutputFile(argv[i]);
      continue;
    }
    if (strcmp(argv[i] + strlen(argv[i]) - 3, ".yz")) {
      Log::e("Unknown argument " + string(argv[i]));
      exit(1);
    }
    compilerArguments.addSourceFile(argv[i]);
  }
  
  return compilerArguments;
}

