//
//  ArrayElementAssignment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

llvm::Value* ArrayElementAssignment::generateElementAssignment(IRGenerationContext& context,
                                                               const IType* elementType,
                                                               const IExpression* assignToExpression,
                                                               llvm::Value* elementStore,
                                                               int line) {
  if (elementType->isOwner()) {
    return generateOwnerElementAssignment(context,
                                          elementType,
                                          assignToExpression,
                                          elementStore,
                                          line);
  }
  
  if (elementType->isReference()) {
    return generateReferenceElementAssignment(context,
                                              elementType,
                                              assignToExpression,
                                              elementStore,
                                              line);
  }
  
  assert(elementType->isPrimitive());
  return generatePrimitiveElementAssignment(context,
                                            elementType,
                                            assignToExpression,
                                            elementStore,
                                            line);

}

llvm::Value* ArrayElementAssignment::
generateOwnerElementAssignment(IRGenerationContext& context,
                               const IType* elementType,
                               const IExpression* assignToExpression,
                               llvm::Value* elementStore,
                               int line) {
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  llvm::PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* null = ConstantPointerNull::get(int8Pointer);
  ((const IOwnerType*) elementType)->free(context, elementLoaded, null, line);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generateReferenceElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   const IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  const IReferenceType* referenceType = (const IReferenceType*) elementType;
  referenceType->decrementReferenceCount(context, elementLoaded);
  referenceType->incrementReferenceCount(context, newValue);

  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generatePrimitiveElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   const IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

