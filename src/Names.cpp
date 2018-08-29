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

string Names::getTextStreamControllerFullName() {
  return getIOPackageName() + ".CTextStreamWriter";
}

string Names::getStreamMethodName() {
  return "getStream";
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

string Names::getMemoryPoolNonEmptyExceptionName() {
  return "MMemoryPoolNonEmpty";
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

string Names::getDataPackageName() {
  return "wisey.data";
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

string Names::getCMemoryPoolName() {
  return "CMemoryPool";
}

string Names::getCMemoryPoolFullName() {
  return getLangPackageName() + "." + getCMemoryPoolName();
}

string Names::getContextManagerName() {
  return "CContextManager";
}

string Names::getPallocateMethodName() {
  return "pallocate";
}

string Names::getClearMethodName() {
  return "clear";
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

string Names::getThrowStackOverflowMethodName() {
  return "throwStackOverflowException";
}

string Names::getCallStackSizeConstantName() {
  return "CALL_STACK_SIZE";
}
