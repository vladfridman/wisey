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

string Names::getNPEModelName() {
  return "MNullPointerException";
}

string Names::getReferenceCountExceptionName() {
  return "MReferenceCountException";
}

string Names::getIProgramName() {
  return "IProgram";
}

string Names::getLangPackageName() {
  return "wisey.lang";
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

string Names::getThreadSetObjectAndMethod() {
  return "setObjectAndMethod";
}

string Names::getThreadPushStack() {
  return "pushStack";
}

string Names::getThreadPopStack() {
  return "popStack";
}

string Names::getCurrentObjectVariableName() {
  return "__currentObject";
}

string Names::getCurrentMethodVariableName() {
  return "__currentMethod";
}
