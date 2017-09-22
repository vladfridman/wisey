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
    method1->getReturnType() == method2->getReturnType();
}

FunctionType* IMethodDescriptor::getLLVMFunctionType(IMethodDescriptor* method,
                                                     IRGenerationContext& context,
                                                     IObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  if (!method->isStatic()) {
    argumentTypes.push_back(object->getLLVMType(llvmContext));
  }
  for (MethodArgument* methodArgument : method->getArguments()) {
    const IType* type = methodArgument->getType();
    argumentTypes.push_back(type->getLLVMType(llvmContext));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = method->getReturnType()->getLLVMType(llvmContext);
  return FunctionType::get(llvmReturnType, argTypesArray, false);
}

void IMethodDescriptor::extractHeaderFromDescriptor(const IMethodDescriptor* method, 
                                                    iostream& stream) {
  stream << "  ";
  if (method->isStatic()) {
    stream << "static ";
  }
  stream << method->getReturnType()->getName() << " " << method->getName() << "(";
  vector<MethodArgument*> arguments = method->getArguments();
  for (MethodArgument* argument : arguments) {
    stream << endl << "    " << argument->getType()->getName() << " " << argument->getName();
    if (argument != arguments.at(arguments.size()-1)) {
      stream << ",";
    }
  }
  stream << ");" << endl;
}
