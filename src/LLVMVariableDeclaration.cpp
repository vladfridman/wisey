//
//  LLVMVariableDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMVariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMVariableDeclaration::LLVMVariableDeclaration(const ILLVMTypeSpecifier* typeSpecifier,
                                                 Identifier* identifier,
                                                 IExpression* assignmentExpression,
                                                 int line) :
mTypeSpecifier(typeSpecifier),
mIdentifier(identifier),
mAssignmentExpression(assignmentExpression),
mLine(line) { }

LLVMVariableDeclaration::~LLVMVariableDeclaration() {
  delete mIdentifier;
  delete mTypeSpecifier;
  if (mAssignmentExpression != NULL) {
    delete mAssignmentExpression;
  }
}

LLVMVariableDeclaration* LLVMVariableDeclaration::create(const ILLVMTypeSpecifier *typeSpecifier,
                                                         Identifier* identifier,
                                                         int line) {
  return new LLVMVariableDeclaration(typeSpecifier, identifier, NULL, line);
}

LLVMVariableDeclaration* LLVMVariableDeclaration::createWithAssignment(const ILLVMTypeSpecifier*
                                                                       typeSpecifier,
                                                                       Identifier* identifier,
                                                                       IExpression*
                                                                       assignmentExpression,
                                                                       int line) {
  return new LLVMVariableDeclaration(typeSpecifier, identifier, assignmentExpression, line);
}

Value* LLVMVariableDeclaration::generateIR(IRGenerationContext& context) const {
  mTypeSpecifier->getType(context)->createLocalVariable(context, mIdentifier->getIdentifierName());
  
  if (mAssignmentExpression == NULL) {
    return NULL;
  }
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = mIdentifier->getVariable(context, arrayIndices);
  return variable->generateAssignmentIR(context, mAssignmentExpression, arrayIndices, mLine);
}

const ILLVMTypeSpecifier* LLVMVariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* LLVMVariableDeclaration::getIdentifier() const {
  return mIdentifier;
}