//
//  LLVMFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMFunction.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMFunction::LLVMFunction(std::string name,
                           const ILLVMType* returnType,
                           std::vector<const LLVMFunctionArgument*> arguments,
                           CompoundStatement* comoundStatement,
                           int line) :
mName(name),
mReturnType(returnType),
mArguments(arguments),
mComoundStatement(comoundStatement),
mLine(line) {
}

LLVMFunction::~LLVMFunction() {
}

Value* LLVMFunction::generateIR(IRGenerationContext& context, const IObjectType* objectType) const {
  return NULL;
}

bool LLVMFunction::isConstant() const {
  return false;
}

bool LLVMFunction::isField() const {
  return false;
}

bool LLVMFunction::isMethod() const {
  return false;
}

bool LLVMFunction::isStaticMethod() const {
  return false;
}

bool LLVMFunction::isMethodSignature() const {
  return false;
}

bool LLVMFunction::isLLVMFunction() const {
  return true;
}
