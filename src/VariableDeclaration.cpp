//
//  VariableDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Assignment.hpp"
#include "wisey/Environment.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/LocalArrayReferenceVariable.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

VariableDeclaration::VariableDeclaration(const ITypeSpecifier* typeSpecifier,
                                         Identifier* identifier,
                                         IExpression* assignmentExpression,
                                         int line) :
mTypeSpecifier(typeSpecifier),
mIdentifier(identifier),
mAssignmentExpression(assignmentExpression),
mLine(line) { }

VariableDeclaration::~VariableDeclaration() {
  delete mIdentifier;
  delete mTypeSpecifier;
  if (mAssignmentExpression != NULL) {
    delete mAssignmentExpression;
  }
}

VariableDeclaration* VariableDeclaration::create(const ITypeSpecifier *typeSpecifier,
                                                 Identifier* identifier,
                                                 int line) {
  return new VariableDeclaration(typeSpecifier, identifier, NULL, line);
}

VariableDeclaration* VariableDeclaration::createWithAssignment(const ITypeSpecifier *typeSpecifier,
                                                               Identifier* identifier,
                                                               IExpression* assignmentExpression,
                                                               int line) {
  return new VariableDeclaration(typeSpecifier, identifier, assignmentExpression, line);
}

void VariableDeclaration::generateIR(IRGenerationContext& context) const {
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

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getIdentifier() const {
  return mIdentifier;
}
