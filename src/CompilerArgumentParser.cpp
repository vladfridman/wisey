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

CompilerArguments CompilerArgumentParser::parse(vector<string> argumnets) const {
  CompilerArguments compilerArguments;

  if (argumnets.size() == 0) {
    printSyntaxAndExit();
  }
  
  for (vector<string>::iterator iterator = argumnets.begin();
       iterator != argumnets.end();
       iterator++) {
    string argument = *iterator;
    if (!argument.compare("--help") || !argument.compare("-h")) {
      printSyntaxAndExit();
    }
    if (!argument.compare("--emit-llvm") || !argument.compare("-e")) {
      compilerArguments.setShouldPrintAssembly(true);
      continue;
    }
    if (!argument.compare("--verbouse") || !argument.compare("-v")) {
      compilerArguments.setVerbouse(true);
      continue;
    }
    if ((!argument.compare("--output") || !argument.compare("-o")) &&
        argument == argumnets.back()) {
      Log::errorNoSourceFile("You need to specify the output file name after \"" + argument + "\"");
      exit(1);
    }
    if (!argument.compare("--output") || !argument.compare("-o")) {
      iterator++;
      compilerArguments.setOutputFile(*iterator);
      continue;
    }
    if ((!argument.compare("--headers") || !argument.compare("-H")) &&
        argument == argumnets.back()) {
      Log::errorNoSourceFile("You need to specify the header file name after \"" +
                             string(argument) + "\"");
      exit(1);
    }
    if (!argument.compare("--headers") || !argument.compare("-H")) {
      iterator++;
      compilerArguments.setHeaderFile(*iterator);
      continue;
    }
    if (!argument.compare("--no-output") || !argument.compare("-n")) {
      compilerArguments.setShouldOutput(false);
      continue;
    }
    if (!argument.compare("--destructor-debug") || !argument.compare("-d")) {
      compilerArguments.setDestructorDebug(true);
      continue;
    }
    if (argument.substr(argument.length() - 3).compare(".yz")) {
      Log::errorNoSourceFile("Unknown argument " + argument);
      exit(1);
    }
    compilerArguments.addSourceFile(argument);
  }
  
  return compilerArguments;
}

