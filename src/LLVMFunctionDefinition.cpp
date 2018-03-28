//
//  LLVMFunctionDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMFunctionDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMFunctionDefinition::LLVMFunctionDefinition(string name,
                                               const ITypeSpecifier* returnSpecifier,
                                               LLVMVariableList arguments,
                                               CompoundStatement* compoundStatement,
                                               int line) :
mName(name),
mReturnSpecifier(returnSpecifier),
mArguments(arguments),
mCompoundStatement(compoundStatement),
mLine(line) {
}

LLVMFunctionDefinition::~LLVMFunctionDefinition() {
  delete mReturnSpecifier;
  for (LLVMVariableDeclaration* variable : mArguments) {
    delete variable;
  }
  mArguments.clear();
  delete mCompoundStatement;
}

LLVMFunction* LLVMFunctionDefinition::define(IRGenerationContext& context,
                                             const IObjectType* objectType) const {
  const IType* returnType = mReturnSpecifier->getType(context);
  
  vector<const LLVMFunctionArgument*> arguments = createArgumnetList(context);
  vector<const IType*> arugmentTypes;
  for (const LLVMFunctionArgument* argument : arguments) {
    arugmentTypes.push_back(argument->getType());
  }
  LLVMFunctionType* functionType = context.getLLVMFunctionType(returnType, arugmentTypes);
  return new LLVMFunction(mName, functionType, returnType, arguments, mCompoundStatement, mLine);
}

bool LLVMFunctionDefinition::isConstant() const {
  return false;
}

bool LLVMFunctionDefinition::isField() const {
  return false;
}

bool LLVMFunctionDefinition::isMethod() const {
  return false;
}

bool LLVMFunctionDefinition::isStaticMethod() const {
  return false;
}

bool LLVMFunctionDefinition::isMethodSignature() const {
  return false;
}

bool LLVMFunctionDefinition::isLLVMFunction() const {
  return true;
}

vector<const LLVMFunctionArgument*> LLVMFunctionDefinition::createArgumnetList(IRGenerationContext&
                                                                               context) const {
  vector<const LLVMFunctionArgument*> arguments;
  
  for (LLVMVariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    const IType* type = (**iterator).getTypeSpecifier()->getType(context);
    string name = (**iterator).getIdentifier()->getIdentifierName();
    LLVMFunctionArgument* methodArgument = new LLVMFunctionArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  return arguments;
}
