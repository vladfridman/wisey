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

string Names::getFileControllerFullName() {
  return getIOPackageName() + ".CFile";
}

string Names::getFileStructMethodName() {
  return "getFileStruct";
}

string Names::getExceptionInterfaceFullName() {
  return getLangPackageName() + "." + getExceptionInterfaceName();
}

string Names::getExceptionInterfaceName() {
  return "IException";
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

string Names::getInterfaceNotBoundExceptionName() {
  return "MInterfaceNotBoundException";
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
  return "wisey.threads";
}

string Names::getIOPackageName() {
  return "wisey.io";
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

string Names::getGetContextManagerMethodName() {
  return "getContextManager";
}

string Names::getCallStackControllerName() {
  return "CCallStack";
}

string Names::getCallStackControllerFullName() {
  return getThreadsPackageName() + "." + getCallStackControllerName();
}

string Names::getCPoolMapName() {
  return "CPoolMap";
}

string Names::getCPoolMapFullName() {
  return getLangPackageName() + "." + getCPoolMapName();
}

string Names::getContextManagerName() {
  return "CContextManager";
}

string Names::getGetPoolMethodName() {
  return "getPool";
}

string Names::getCreatePoolMethodName() {
  return "createPool";
}

string Names::getContextManagerFullName() {
  return getThreadsPackageName() + "." + getContextManagerName();
}

string Names::getGetInstanceMethodName() {
  return "getInstance";
}

string Names::getSetInstanceMethodName() {
  return "setInstance";
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
