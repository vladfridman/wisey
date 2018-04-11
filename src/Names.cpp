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

string Names::getMainThreadShortName() {
  return "TMainThread";
}

string Names::getMainThreadFullName() {
  return getLangPackageName() + "." + getMainThreadShortName();
}

string Names::getProgramResultShortName() {
  return "TMainThread.MProgramResult";
}

string Names::getThreadSetObjectAndMethod() {
  return "setObjectAndMethod";
}

string Names::getCallStackControllerName() {
  return "CCallStack";
}

string Names::getCallStackControllerFullName() {
  return getLangPackageName() + "." + getCallStackControllerName();
}

string Names::getCallStackMethodName() {
  return "getCallStack";
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

string Names::getCheckConcealedMethodCallFunctionName() {
  return "wisey.lang.threads.IThread.checkConcealedMethodCall";
}

string Names::getCheckRevealedMethodCallFunctionName() {
  return "wisey.lang.threads.IThread.checkRevealedMethodCall";
}
