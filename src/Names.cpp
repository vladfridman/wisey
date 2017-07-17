//
//  Names.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Names.hpp"

using namespace std;
using namespace wisey;

string Names::getNPECheckFunctionName() {
  return "__checkForNullAndThrow";
}

string Names::getNPEModelName() {
  return "MNullPointerException";
}

string Names::getNPEModelFullName() {
  return getLangPackageName() + "." + getNPEModelName();
}

string Names::getIProgramName() {
  return "IProgram";
}

string Names::getIProgramFullName() {
  return getLangPackageName() + "." + getIProgramName();
}

string Names::getLangPackageName() {
  return "wisey.lang";
}
