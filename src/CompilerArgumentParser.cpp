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
  cerr << "Syntax: wiseyc "
          "[-d|--destructor-debug] "
          "[-e|--emit-llvm] "
          "[-h|--help] "
          "[-v|--verbouse] "
          "[-H|--headers <header_file.yzh>] "
          "[-o|--output <object_file.o>] "
          "[-n|--no-output] "
          "<source_file.yz>..." << endl;
  exit(1);
}

CompilerArguments CompilerArgumentParser::parse(int argc, char **argv) const {
  CompilerArguments compilerArguments;

  if (argc <= 1) {
    printSyntaxAndExit();
  }
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      printSyntaxAndExit();
    }
    if (!strcmp(argv[i], "--emit-llvm") || !strcmp(argv[i], "-e")) {
      compilerArguments.setShouldPrintAssembly(true);
      continue;
    }
    if (!strcmp(argv[i], "--verbouse") || !strcmp(argv[i], "-v")) {
      compilerArguments.setVerbouse(true);
      continue;
    }
    if ((!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) && i == argc - 1) {
      Log::e("You need to specify the output file name after \"" + string(argv[i]) + "\"");
      exit(1);
    }
    if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) {
      i++;
      compilerArguments.setOutputFile(argv[i]);
      continue;
    }
    if ((!strcmp(argv[i], "--headers") || !strcmp(argv[i], "-H")) && i == argc - 1) {
      Log::e("You need to specify the header file name after \"" + string(argv[i]) + "\"");
      exit(1);
    }
    if (!strcmp(argv[i], "--headers") || !strcmp(argv[i], "-H")) {
      i++;
      compilerArguments.setHeaderFile(argv[i]);
      continue;
    }
    if (!strcmp(argv[i], "--no-output") || !strcmp(argv[i], "-n")) {
      compilerArguments.setShouldOutput(false);
      continue;
    }
    if (!strcmp(argv[i], "--destructor-debug") || !strcmp(argv[i], "-d")) {
      compilerArguments.setDestructorDebug(true);
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

