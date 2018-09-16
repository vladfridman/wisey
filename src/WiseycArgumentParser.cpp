//
//  WiseycArgumentParser.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/WiseycArgumentParser.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

WiseycArgumentParser::WiseycArgumentParser() {}

WiseycArgumentParser::~WiseycArgumentParser() {}

void WiseycArgumentParser::printSyntaxAndExit() const {
  cerr << "Syntax: wiseyc "
  "[-d|--destructor-debug] "
  "[-e|--emit-llvm] "
  "[-h|--help] "
  "[-v|--verbouse] "
  "[-o|--output <runnable_file_name>] "
  "[-n|--no-output] "
  "[--no-optimization] "
  "[-L<path_to_library>] "
  "[-l<library_name_to_link>] "
  "<source_file.yz>..." << endl;
  exit(1);
}

CompilerArguments WiseycArgumentParser::parse(vector<string> arguments) const {
  CompilerArguments compilerArguments;
  
  if (arguments.size() == 0) {
    printSyntaxAndExit();
  }
  
  for (vector<string>::iterator iterator = arguments.begin();
       iterator != arguments.end();
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
        argument == arguments.back()) {
      Log::errorNoSourceFile("You need to specify the output file name after \"" + argument + "\"");
      exit(1);
    }
    if (!argument.compare("--output") || !argument.compare("-o")) {
      iterator++;
      compilerArguments.setOutputFile(*iterator);
      continue;
    }
    if (argument.find("-L") == 0) {
      compilerArguments.addLibraryPath(argument);
      continue;
    }
    if (argument.find("-l") == 0) {
      compilerArguments.addLibraryName(argument);
      continue;
    }
    if (!argument.compare("--no-output") || !argument.compare("-n")) {
      compilerArguments.setShouldOutput(false);
      continue;
    }
    if (!argument.compare("--no-optimization")) {
      compilerArguments.setShouldOptimize(false);
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

