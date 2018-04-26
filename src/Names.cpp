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

string Names::getMainThreadWorkerShortName() {
  return "MMainThreadWorker";
}

string Names::getNPEModelName() {
  return "MNullPointerException";
}

string Names::getReferenceCountExceptionName() {
  return "MReferenceCountException";
}

string Names::getCastExceptionName() {
  return "MCastException";
}

string Names::getArrayIndexOutOfBoundsModelName() {
  return "MArrayIndexOutOfBoundsException";
}

string Names::getIProgramName() {
  return "IProgram";
}

string Names::getLangPackageName() {
  return "wisey.lang";
}

string Names::getThreadsPackageName() {
  return "wisey.lang.threads";
}

string Names::getStdErrName() {
  return "__stderrp";
}

string Names::getThreadInterfaceName() {
  return "IThread";
}

string Names::getThreadInterfaceFullName() {
  return getThreadsPackageName() + "." + getThreadInterfaceName();
}

string Names::getCallStackControllerName() {
  return "CCallStack";
}

string Names::getCallStackControllerFullName() {
  return getLangPackageName() + "." + getCallStackControllerName();
}

string Names::getPushStackMethodName() {
  return "pushStack";
}

string Names::getPopStackMethoName() {
  return "popStack";
}

string Names::getSetLineNumberMethodName() {
  return "setLine";
}

string Names::getCurrentObjectVariableName() {
  return "__currentObject";
}

string Names::getCurrentMethodVariableName() {
  return "__currentMethod";
}
