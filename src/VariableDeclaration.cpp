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
#include "wisey/LocalArrayVariable.hpp"
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

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  const IType* type = mTypeSpecifier->getType(context);
  TypeKind typeKind = type->getTypeKind();
  if (typeKind == PRIMITIVE_TYPE) {
    allocatePrimitive(context, (const IPrimitiveType*) type);
  } else if (typeKind == ARRAY_TYPE) {
    allocateArray(context, (const wisey::ArrayType*) type);
  } else if (IType::isOwnerType(type)) {
    allocateOwner(context, (const IObjectOwnerType*) type);
  } else {
    allocateReference(context, (const IObjectType*) type);
  }
  
  if (mAssignmentExpression == NULL) {
    return NULL;
  }
  
  IVariable* variable = IVariable::getVariable(context, mIdentifier->getName());
  variable->generateAssignmentIR(context, mAssignmentExpression, mLine);
  
  return NULL;
}

void VariableDeclaration::allocatePrimitive(IRGenerationContext& context,
                                            const IPrimitiveType* type) const {
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mIdentifier->getName());

  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(mIdentifier->getName(),
                                                                type,
                                                                alloc);
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

void VariableDeclaration::allocateArray(IRGenerationContext& context,
                                        const wisey::ArrayType* type) const {
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mIdentifier->getName());
  
  LocalArrayVariable* variable = new LocalArrayVariable(mIdentifier->getName(), type, alloc);
  context.getScopes().setVariable(variable);
}

void VariableDeclaration::allocateOwner(IRGenerationContext& context,
                                        const IObjectOwnerType* type) const {
  string variableName = mIdentifier->getName();
  PointerType* llvmType = type->getLLVMType(context.getLLVMContext());
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void VariableDeclaration::allocateReference(IRGenerationContext& context,
                                            const IObjectType* type) const {
  string variableName = mIdentifier->getName();
  PointerType* llvmType = (PointerType*) type->getLLVMType(context.getLLVMContext());

  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);

  IVariable* uninitializedVariable = new LocalReferenceVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getIdentifier() const {
  return mIdentifier;
}
