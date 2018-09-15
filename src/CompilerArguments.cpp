//
//  CompilerArguments.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/CompilerArguments.hpp"

using namespace std;
using namespace wisey;

CompilerArguments::CompilerArguments()  :
mOutputFile(""),
mHeaderFile(""),
mShouldPrintAssembly(false),
mIsVerbouse(false),
mShouldOutput(true),
mIsDestructorDebugOn(false),
mShouldOptimize(true) {
}

CompilerArguments::~CompilerArguments() {
}

string CompilerArguments::getOutputFile() const {
  return mOutputFile;
}

void CompilerArguments::setOutputFile(string outputFile)  {
  mOutputFile = outputFile;
}

string CompilerArguments::getHeaderFile() const {
  return mHeaderFile;
}

void CompilerArguments::setHeaderFile(string headerFile) {
  mHeaderFile = headerFile;
}

bool CompilerArguments::shouldPrintAssembly() const {
  return mShouldPrintAssembly;
}

void CompilerArguments::setShouldPrintAssembly(bool value) {
  mShouldPrintAssembly = value;
}

bool CompilerArguments::isVerbouse() const {
  return mIsVerbouse;
}

void CompilerArguments::setVerbouse(bool value) {
  mIsVerbouse = value;
}

bool CompilerArguments::shouldOutput() const {
  return mShouldOutput;
}

void CompilerArguments::setShouldOutput(bool value) {
  mShouldOutput = value;
}

vector<string> CompilerArguments::getSourceFiles() const {
  return mSourceFiles;
}

void CompilerArguments::addSourceFile(string sourceFile) {
  mSourceFiles.push_back(sourceFile);
}

bool CompilerArguments::isDestructorDebugOn() const {
  return mIsDestructorDebugOn;
}

void CompilerArguments::setDestructorDebug(bool value) {
  mIsDestructorDebugOn = value;
}

bool CompilerArguments::shouldOptimize() const {
  return mShouldOptimize;
}

void CompilerArguments::setShouldOptimize(bool value) {
  mShouldOptimize = value;
}

void CompilerArguments::addLibraryPath(string path) {
  mLibraryPaths.push_back(path);
}

vector<string> CompilerArguments::getLibraryPaths() const {
  return mLibraryPaths;
}

void CompilerArguments::addLibraryName(string name) {
  mLibraryNames.push_back(name);
}

vector<string> CompilerArguments::getLibraryNames() const {
  return mLibraryNames;
}

void CompilerArguments::addAdditionalObject(string path) {
  mAdditionalObjects.push_back(path);
}

vector<string> CompilerArguments::getAdditionalObjects() const {
  return mAdditionalObjects;
}

string CompilerArguments::getForYzc() const {
  string command = "";
  if (!mShouldOptimize) {
    command += "--no-optimization ";
  }
  if (mIsDestructorDebugOn) {
    command += "--destructor-debug ";
  }
  if (!mShouldOutput) {
    command += "--no-output ";
  }
  if (mShouldPrintAssembly) {
    command += "--emit-llvm ";
  }
  if (mHeaderFile.size()) {
    command += "-H " + mHeaderFile + " ";
  }
  if (mIsVerbouse) {
    command += "--verbouse ";
  }
  if (mShouldOutput && mOutputFile.size()) {
    command += "--output " + mOutputFile + " ";
  }
  for (string sourceFile : mSourceFiles) {
    command += sourceFile + " ";
  }
  return command;
}
