//
//  WiseylibcArgumentParser.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/Log.hpp"
#include "wisey/WiseylibcArgumentParser.hpp"

using namespace std;
using namespace wisey;

WiseylibcArgumentParser::WiseylibcArgumentParser() {}

WiseylibcArgumentParser::~WiseylibcArgumentParser() {}

void WiseylibcArgumentParser::printSyntaxAndExit() const {
  cerr << "Syntax: wiseylibc "
  "[-d|--destructor-debug] "
  "[-e|--emit-llvm] "
  "[-h|--help] "
  "[-v|--verbouse] "
  "[-H|--headers <header_file.yz>] "
  "[-o|--output <library_file_name.so>] "
  "[-n|--no-output] "
  "[--no-optimization] "
  "[-A<additional_object_to_link>] "
  "<source_file.yz>..." << endl;
  exit(1);
}

CompilerArguments WiseylibcArgumentParser::parse(vector<string> argumnets) const {
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
    if (argument.find("-A") == 0) {
      compilerArguments.addAdditionalObject(argument.substr(2));
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


