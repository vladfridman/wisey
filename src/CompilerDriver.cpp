//
//  CompilerDriver.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <sys/stat.h>

#include "wisey/CompilerArgumentParser.hpp"
#include "wisey/CompilerDriver.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

CompilerDriver::CompilerDriver() {
}

CompilerDriver::~CompilerDriver() {
}

void CompilerDriver::compileRunnable(int argc, char **argv) {
  prepareForRunnable();

  vector<string> arguments;
  for (int i = 1; i < argc; i++) {
    arguments.push_back(argv[i]);
  }
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguments =
  compilerArgumentParser.parse(arguments, CompilerMode::WISEYC);
  
  Log::setLogLevel(compilerArguments.isVerbouse() ? INFOLEVEL : ERRORLEVEL);
  
  string objectFileName = "runnable.o";
  string runnableFileName = "runnable";
  if (compilerArguments.shouldOutput() && compilerArguments.getOutputFile().size()) {
    runnableFileName = compilerArguments.getOutputFile();
    objectFileName = runnableFileName + ".o";
  }

  compilerArguments.setOutputFile(objectFileName);
  string command = "yzc " + compilerArguments.toString();
  command += " " + mWiseyHeaders;
  if (executeCommand(command) || !compilerArguments.shouldOutput()) {
    return;
  }

  command = "ld -macosx_version_min " + mOsVersion + " -L" + mWiseyLib + " -lwisey -lc++ -lSystem ";
  command += "-arch " + mArchitecture + " ";
  command += "-o " + runnableFileName + " " + objectFileName;
  executeCommand(command);
}

void CompilerDriver::compileLibrary(int argc, char **argv) {
  prepareForLibrary();
  
  vector<string> arguments;
  for (int i = 1; i < argc; i++) {
    arguments.push_back(argv[i]);
  }
  CompilerArgumentParser compilerArgumentParser;
  CompilerArguments compilerArguments =
  compilerArgumentParser.parse(arguments, CompilerMode::WISEYLIBC);
  
  Log::setLogLevel(compilerArguments.isVerbouse() ? INFOLEVEL : ERRORLEVEL);
  
  string objectFileName = "library.o";
  string libraryFileName = "library.so";
  if (compilerArguments.shouldOutput() && compilerArguments.getOutputFile().size()) {
    libraryFileName = compilerArguments.getOutputFile();
    objectFileName = libraryFileName.find_last_of(".") == string::npos
    ? libraryFileName + ".o"
    : libraryFileName.substr(0, libraryFileName.find_last_of(".")) + ".o";
  }
  
  compilerArguments.setOutputFile(objectFileName);
  string command = "yzc " + compilerArguments.toString();
  if (executeCommand(command) || !compilerArguments.shouldOutput()) {
    return;
  }
  
  command = "ld -macosx_version_min " + mOsVersion + " -lc++ -lSystem -dylib ";
  command += "-arch " + mArchitecture + " ";
  for (string path : compilerArguments.getLibraryPaths()) {
    command += path + " ";
  }
  for (string name : compilerArguments.getLibraryNames()) {
    command += name + " ";
  }
  command += "-o " + libraryFileName + " " + objectFileName;
  executeCommand(command);
}

bool CompilerDriver::prepareForRunnable() {
  mWiseyHome = std::getenv("WISEY_HOME");
  if(!mWiseyHome.length()) {
    cerr << "Environment variable $WISEY_HOME is not defined\n";
    return false;
  }
  
  mWiseyHeaders = mWiseyHome + "/headers/libwisey.yz";
  mWiseyLib = mWiseyHome + "/lib";
  
  if (!checkCommandExists("yzc")) {
    cerr << "Wisey compiler yzc is not found. "
    "Add path to wiseyc to your $PATH environment variable" << endl;
    return false;
  }

  if (!checkFileExists(mWiseyHeaders.c_str())) {
    cerr << "Headers file is not found. Based on $WISEY_HOME environment variable it should be in "
    << mWiseyHeaders << endl;
    return false;
  }
  
  if (!checkFileExists(mWiseyLib.c_str())) {
    cerr << "Wisey library is not found. Based on $WISEY_HOME environment variable it should be in "
    << mWiseyLib << endl;
    return false;
  }
  
  return prepareForLibrary();
}

bool CompilerDriver::prepareForLibrary() {
  if (!checkCommandExists("uname")) {
    cerr << "uname command is not found" << endl;
    return false;
  }
  mArchitecture = getArchitecture();

  if (!checkCommandExists("ld")) {
    cerr << "ld command is not found" << endl;
    return false;
  }
  
  if (!checkCommandExists("sw_vers")) {
    cerr << "sw_vers command is not found" << endl;
    return false;
  }
  mOsVersion = getMacOsXVersion();
  
  return true;
}

bool CompilerDriver::checkFileExists(const char* fileName) const {
  struct stat buffer;
  return (stat(fileName, &buffer) == 0);
}

bool CompilerDriver::checkCommandExists(string check) const {
  string command = "command -v " + check + " > /dev/null 2>&1";
  return !system(command.c_str());
}

string CompilerDriver::getMacOsXVersion() const {
  char line[256];
  FILE* sw_vers = popen("sw_vers -productVersion", "r");
  fgets(line, sizeof(line), sw_vers);
  if (line[strlen(line) - 1] == '\n') {
    line[strlen(line) - 1] = '\0';
  }
  pclose(sw_vers);
  return string(line);
}

string CompilerDriver::getArchitecture() const {
  char line[256];
  FILE* sw_vers = popen("uname -m", "r");
  fgets(line, sizeof(line), sw_vers);
  if (line[strlen(line) - 1] == '\n') {
    line[strlen(line) - 1] = '\0';
  }
  pclose(sw_vers);
  return string(line);
}

int CompilerDriver::executeCommand(string command) const {
  Log::i(command);
  return system(command.c_str());
}
