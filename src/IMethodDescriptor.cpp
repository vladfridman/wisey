//
//  IMethodDescriptor.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

bool IMethodDescriptor::compare(IMethodDescriptor* method1, IMethodDescriptor* method2) {
  vector<MethodArgument*> thisArguments = method1->getArguments();
  vector<MethodArgument*> thatArugments = method2->getArguments();
  if (thisArguments.size() != thatArugments.size()) {
    return false;
  }
  
  vector<MethodArgument*>::const_iterator thatArgumentIterator = thatArugments.begin();
  for (vector<MethodArgument*>::const_iterator thisArgumentIterator = thisArguments.begin();
       thisArgumentIterator != thisArguments.end();
       thisArgumentIterator++, thatArgumentIterator++) {
    MethodArgument* thisArgument = *thisArgumentIterator;
    MethodArgument* thatArgument = *thatArgumentIterator;
    if (thisArgument->getType() != thatArgument->getType()) {
      return false;
    }
  }
  
  return !method1->getName().compare(method2->getName()) &&
    method1->getReturnType() == method2->getReturnType() &&
    method1->isExposed() == method2->isExposed();
}

FunctionType* IMethodDescriptor::getLLVMFunctionType(IRGenerationContext& context,
                                                     const IMethodDescriptor* method,
                                                     const IObjectType* object,
                                                     int line) {
  vector<Type*> argumentTypes;
  if (!method->isStatic()) {
    argumentTypes.push_back(object->getLLVMType(context));
  }
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), line);
  argumentTypes.push_back(threadInterface->getLLVMType(context));
  Controller* callStack = context.getController(Names::getCallStackControllerFullName(), line);
  argumentTypes.push_back(callStack->getLLVMType(context));
  
  for (MethodArgument* methodArgument : method->getArguments()) {
    const IType* type = methodArgument->getType();
    argumentTypes.push_back(type->getLLVMType(context));
  }
  Type* llvmReturnType = method->getReturnType()->getLLVMType(context);
  return FunctionType::get(llvmReturnType, argumentTypes, false);
}

void IMethodDescriptor::printDescriptorToStream(const IMethodDescriptor* method,
                                                iostream& stream) {
  stream << "  ";
  if (method->isStatic()) {
    stream << "static ";
  }
  stream << method->getReturnType()->getTypeName() << " " << method->getName() << "(";
  vector<MethodArgument*> arguments = method->getArguments();
  for (MethodArgument* argument : arguments) {
    stream << endl << "    " << argument->getType()->getTypeName() << " " << argument->getName();
    if (argument != arguments.at(arguments.size() - 1)) {
      stream << ",";
    }
  }
  stream << ")";
  
  vector<const Model*> exceptions = method->getThrownExceptions();
  
  if (exceptions.size()) {
    stream << " throws ";
  }
  for (const Model* exception : exceptions) {
    if (exception != exceptions.front()) {
      stream << ", ";
    }
    stream << exception->getTypeName();
  }

  if (method->isExposed()) {
    stream << " exposed";
  }
  if (method->isOverride()) {
    stream << " override";
  }

  stream << ";" << endl;
}
