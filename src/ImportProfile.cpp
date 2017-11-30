//
//  ImportProfile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectType.hpp"
#include "wisey/ImportProfile.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

ImportProfile::ImportProfile(string package) : mPackage(package) {
}

ImportProfile::~ImportProfile() {
}

void ImportProfile::addImport(string shortName, string longName) {
  mImports[shortName] = longName;
}

string ImportProfile::getFullName(string shortName) const {
  if (mImports.count(shortName)) {
    return mImports.at(shortName);
  }
  if (mPackage.length() != 0) {
    return mPackage + "." + shortName;
  }
  
  Log::e("Could not identify packge for object " + shortName);
  exit(1);
}

