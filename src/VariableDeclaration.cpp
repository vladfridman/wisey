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
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
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
                                         Identifier* id) :
mTypeSpecifier(typeSpecifier), mId(id), mAssignmentExpression(NULL) { }

VariableDeclaration::VariableDeclaration(const ITypeSpecifier* typeSpecifier,
                                         Identifier* id,
                                         IExpression* assignmentExpression) :
mTypeSpecifier(typeSpecifier), mId(id), mAssignmentExpression(assignmentExpression) { }

VariableDeclaration::~VariableDeclaration() {
  delete mId;
  delete mTypeSpecifier;
  if (mAssignmentExpression != NULL) {
    delete mAssignmentExpression;
  }
}

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  const IType* type = mTypeSpecifier->getType(context);
  TypeKind typeKind = type->getTypeKind();
  if (typeKind == PRIMITIVE_TYPE) {
    allocatePrimitive(context, (const IPrimitiveType*) type);
  } else if (IType::isOwnerType(type)) {
    allocateOwner(context, (const IObjectOwnerType*) type);
  } else {
    allocateReference(context, (const IObjectType*) type);
  }
  
  if (mAssignmentExpression == NULL) {
    return NULL;
  }
  
  IVariable* variable = IVariable::getVariable(context, mId->getName());
  variable->generateAssignmentIR(context, mAssignmentExpression);
  
  return NULL;
}

void VariableDeclaration::allocatePrimitive(IRGenerationContext& context,
                                            const IPrimitiveType* type) const {
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mId->getName());

  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(mId->getName(), type, alloc);
  context.getScopes().setVariable(variable);

  if (mAssignmentExpression != NULL) {
    return;
  }
  
  Value* value;
  if (llvmType->isFloatTy() || llvmType->isDoubleTy()) {
    value = ConstantFP::get(llvmType, 0.0);
  } else if (llvmType->isIntegerTy()) {
    value = ConstantInt::get(llvmType, 0);
  } else if (llvmType->isPointerTy()) {
    GlobalVariable* nameGlobal =
      context.getModule()->getNamedGlobal(Names::getEmptyStringName());
    ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
    Value* Idx[2];
    Idx[0] = zeroInt32;
    Idx[1] = zeroInt32;
    Type* elementType = nameGlobal->getType()->getPointerElementType();
    
    value = ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
  } else {
    Log::e("Unexpected primitive variable type, can not initialize");
    exit(1);
  }
  IRWriter::newStoreInst(context, value, alloc);
}

void VariableDeclaration::allocateOwner(IRGenerationContext& context,
                                        const IObjectOwnerType* type) const {
  string variableName = mId->getName();
  PointerType* llvmType = type->getLLVMType(context.getLLVMContext());
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void VariableDeclaration::allocateReference(IRGenerationContext& context,
                                            const IObjectType* type) const {
  string variableName = mId->getName();
  PointerType* llvmType = (PointerType*) type->getLLVMType(context.getLLVMContext());

  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);

  IVariable* uninitializedVariable = new LocalReferenceVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getId() const {
  return mId;
}
