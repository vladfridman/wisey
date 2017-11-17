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

string Names::getAdjustReferenceCounterForConcreteObjectUnsafelyFunctionName() {
  return "__adjustReferenceCounterForConcreteObjectUnsafely";
}

string Names::getAdjustReferenceCounterForInterfaceFunctionName() {
  return "__adjustReferenceCounterForInterface";
}

string Names::getNPEModelName() {
  return "MNullPointerException";
}

string Names::getNPEModelFullName() {
  return getLangPackageName() + "." + getNPEModelName();
}

string Names::getDestroyedObjectStillInUseName() {
  return "MDestroyedObjectStillInUseException";
}

string Names::getDestroyedObjectStillInUseFullName() {
  return getLangPackageName() + "." + getDestroyedObjectStillInUseName();
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
