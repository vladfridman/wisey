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

LLVMVariableDeclaration::LLVMVariableDeclaration(const ITypeSpecifier* typeSpecifier,
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

LLVMVariableDeclaration* LLVMVariableDeclaration::create(const ITypeSpecifier *typeSpecifier,
                                                         Identifier* identifier,
                                                         int line) {
  return new LLVMVariableDeclaration(typeSpecifier, identifier, NULL, line);
}

LLVMVariableDeclaration* LLVMVariableDeclaration::createWithAssignment(const ITypeSpecifier*
                                                                       typeSpecifier,
                                                                       Identifier* identifier,
                                                                       IExpression*
                                                                       assignmentExpression,
                                                                       int line) {
  return new LLVMVariableDeclaration(typeSpecifier, identifier, assignmentExpression, line);
}

void LLVMVariableDeclaration::generateIR(IRGenerationContext& context) const {
  mTypeSpecifier->getType(context)->createLocalVariable(context,
                                                        mIdentifier->getIdentifierName(),
                                                        mLine);
  
  if (mAssignmentExpression == NULL) {
    return;
  }
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = mIdentifier->getVariable(context, arrayIndices);
  variable->generateAssignmentIR(context, mAssignmentExpression, arrayIndices, mLine);
}

const ITypeSpecifier* LLVMVariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* LLVMVariableDeclaration::getIdentifier() const {
  return mIdentifier;
}

int LLVMVariableDeclaration::getLine() const {
  return mLine;
}
