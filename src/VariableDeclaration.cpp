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

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  const IType* type = mTypeSpecifier->getType(context);
  TypeKind typeKind = type->getTypeKind();
  if (IType::isPrimitveType(type)) {
    allocatePrimitive(context, (const IPrimitiveType*) type);
  } else if (typeKind == ARRAY_TYPE) {
    allocateArray(context, (const wisey::ArrayType*) type);
  } else if (typeKind == ARRAY_OWNER_TYPE) {
    allocateArrayOwner(context, (const wisey::ArrayOwnerType*) type);
  } else if (IType::isOwnerType(type)) {
    allocateOwner(context, (const IObjectOwnerType*) type);
  } else {
    assert(IType::isReferenceType(type));
    allocateReference(context, (const IObjectType*) type);
  }
  
  if (mAssignmentExpression == NULL) {
    return NULL;
  }
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = mIdentifier->getVariable(context, arrayIndices);
  variable->generateAssignmentIR(context, mAssignmentExpression, arrayIndices, mLine);
  
  return NULL;
}

void VariableDeclaration::allocatePrimitive(IRGenerationContext& context,
                                            const IPrimitiveType* type) const {
  Type* llvmType = type->getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");

  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(mIdentifier->getIdentifierName(),
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
  llvm::PointerType* llvmType = type->getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayReferenceVariable(mIdentifier->getIdentifierName(),
                                                        type,
                                                        alloc);
  context.getScopes().setVariable(variable);
}

void VariableDeclaration::allocateArrayOwner(IRGenerationContext& context,
                                             const wisey::ArrayOwnerType* type) const {
  llvm::PointerType* llvmType = type->getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayOwnerVariable(mIdentifier->getIdentifierName(), type, alloc);
  context.getScopes().setVariable(variable);
}

void VariableDeclaration::allocateOwner(IRGenerationContext& context,
                                        const IObjectOwnerType* type) const {
  string variableName = mIdentifier->getIdentifierName();
  PointerType* llvmType = type->getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void VariableDeclaration::allocateReference(IRGenerationContext& context,
                                            const IObjectType* type) const {
  string variableName = mIdentifier->getIdentifierName();
  PointerType* llvmType = (PointerType*) type->getLLVMType(context);

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
