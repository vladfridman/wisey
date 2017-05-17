//
//  VariableDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Assignment.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/LocalStackVariable.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  Value* value = mTypeSpecifier.getType(context)->getTypeKind() == PRIMITIVE_TYPE
    ? allocateOnStack(context)
    : allocateOnHeap(context);
  
  if (mAssignmentExpression != NULL) {
    Assignment assignment(mId, *mAssignmentExpression);
    assignment.generateIR(context);
  }
  
  return value;
}

Value* VariableDeclaration::allocateOnStack(IRGenerationContext& context) const {
  IType* type = mTypeSpecifier.getType(context);
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mId.getName());
  
  LocalStackVariable* variable = new LocalStackVariable(mId.getName(), type, alloc);
  context.getScopes().setVariable(variable);

  return alloc;
}

Value* VariableDeclaration::allocateOnHeap(IRGenerationContext& context) const {

  string variableName = mId.getName();
  IType* type = mTypeSpecifier.getType(context);
  
  LocalHeapVariable* uninitializedHeapVariable = new LocalHeapVariable(variableName, type, NULL);
  context.getScopes().setVariable(uninitializedHeapVariable);

  return NULL;
}

const ITypeSpecifier& VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier& VariableDeclaration::getId() const {
  return mId;
}
