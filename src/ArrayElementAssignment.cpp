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
                                                               IExpression* assignToExpression,
                                                               llvm::Value* elementStore,
                                                               int line) {
  if (IType::isOwnerType(elementType)) {
    return generateOwnerElementAssignment(context,
                                          elementType,
                                          assignToExpression,
                                          elementStore,
                                          line);
  }
  
  if (IType::isReferenceType(elementType)) {
    return generateReferenceElementAssignment(context,
                                              elementType,
                                              assignToExpression,
                                              elementStore,
                                              line);
  }
  
  assert(IType::isPrimitveType(elementType));
  return generatePrimitiveElementAssignment(context,
                                            elementType,
                                            assignToExpression,
                                            elementStore,
                                            line);

}

llvm::Value* ArrayElementAssignment::
generateOwnerElementAssignment(IRGenerationContext& context,
                               const IType* elementType,
                               IExpression* assignToExpression,
                               llvm::Value* elementStore,
                               int line) {
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, elementLoaded, int8pointer);
  ((const IObjectOwnerType*) elementType)->free(context, bitcast);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generateReferenceElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  const IObjectType* objectType = (const IObjectType*) elementType;
  objectType->decremenetReferenceCount(context, elementLoaded);
  objectType->incremenetReferenceCount(context, newValue);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generatePrimitiveElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

