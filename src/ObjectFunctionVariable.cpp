//
//  ObjectFunctionVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectFunctionVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ObjectFunctionVariable::ObjectFunctionVariable(string name, const IConcreteObjectType* objectType) :
mName(name), mObjectType(objectType) {
}

ObjectFunctionVariable::~ObjectFunctionVariable() {
}

string ObjectFunctionVariable::getName() const {
  return mName;
}

const LLVMFunctionType* ObjectFunctionVariable::getType() const {
  LLVMFunction* function = findFunction();
  
  return function->getType();
}

bool ObjectFunctionVariable::isField() const {
  return false;
}

bool ObjectFunctionVariable::isSystem() const {
  return false;
}

Value* ObjectFunctionVariable::generateIdentifierIR(IRGenerationContext& context) const {
  LLVMFunction* function = findFunction();
  return function->getLLVMFunction(context, mObjectType);
}

Value* ObjectFunctionVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  assert(false);
}

Value* ObjectFunctionVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    std::vector<const IExpression*> arrayIndices,
                                                    int line) {
  assert(false);
}

LLVMFunction* ObjectFunctionVariable::findFunction() const {
  LLVMFunction* function = mObjectType->findLLVMFunction(mName);
  
  if (function == NULL) {
    Log::e_deprecated("LLVM function " + mName + " is not found in object " + mObjectType->getTypeName());
    exit(1);
  }

  return function;
}
