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

string Names::getLangPackageName() {
  return "wisey.lang";
}

string Names::getFreeIfNotNullFunctionName() {
  return "__freeIfNotNull";
}

string Names::getStdErrName() {
  return "__stderrp";
}

string Names::getEmptyStringName() {
  return "__empty.str";
}

string Names::getThreadControllerName() {
  return "CThread";
}

string Names::getThreadControllerFullName() {
  return getLangPackageName() + "." + getThreadControllerName();
}

