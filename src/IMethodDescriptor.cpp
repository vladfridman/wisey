//
//  IMethodDescriptor.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Argument.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

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
  
  for (const Argument* methodArgument : method->getArguments()) {
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
    if (method->isNative()) {
      stream << "::llvm::function ";
    } else {
      stream << "static ";
    }
  }
  stream << method->getReturnType()->getTypeName() << " " << method->getName() << "(";
  vector<const Argument*> arguments = method->getArguments();
  for (const Argument* argument : arguments) {
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

  if (method->isOverride()) {
    stream << " override";
  }

  stream << ";" << endl;
}
