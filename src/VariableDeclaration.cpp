//
//  VariableDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/Assignment.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  AllocaInst *alloc = new AllocaInst(mType.getType(context),
                                     mId.getName().c_str(),
                                     context.getBasicBlock());
  context.setVariable(mId.getName(), alloc);
  if (mAssignmentExpression != NULL) {
    Assignment assignment(mId, *mAssignmentExpression);
    assignment.generateIR(context);
  }
  
  return alloc;
}

const ITypeSpecifier& VariableDeclaration::getType() const {
  return mType;
}

const Identifier& VariableDeclaration::getId() const {
  return mId;
}
