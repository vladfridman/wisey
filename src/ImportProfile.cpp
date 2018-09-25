//
//  ImportProfile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "GlobalStringConstant.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "ImportProfile.hpp"
#include "Log.hpp"
#include "ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ImportProfile::ImportProfile(string package) :
mPackage(package),
mSourceFileName(""),
mSourceFileConstantPointer(NULL) {
}

ImportProfile::~ImportProfile() {
}

void ImportProfile::addImport(string shortName, string longName) {
  mImports[shortName] = longName;
}

string ImportProfile::getFullName(string shortName, int line) const {
  if (mImports.count(shortName)) {
    return mImports.at(shortName);
  }
  if (mPackage.length() != 0) {
    return mPackage + "." + shortName;
  }
  
  Log::e(mSourceFileName, line, "Could not identify packge for object " + shortName);
  throw 1;
}

void ImportProfile::setSourceFileName(IRGenerationContext& context, string sourceFileName) {
  mSourceFileName = sourceFileName;
  if (mSourceFileName.size()) {
    mSourceFileConstantPointer = GlobalStringConstant::get(context, sourceFileName);
  } else {
    mSourceFileConstantPointer = NULL;
  }
}

string ImportProfile::getSourceFileName() const {
  return mSourceFileName;
}

Value* ImportProfile::getSourceFileNamePointer() const {
  return mSourceFileConstantPointer;
}
