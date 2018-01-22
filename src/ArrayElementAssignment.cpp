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
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  ((const IObjectOwnerType*) elementType)->free(context, elementLoaded);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generateReferenceElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  const IObjectType* objectType = (const IObjectType*) elementType;
  objectType->decrementReferenceCount(context, elementLoaded);
  objectType->incrementReferenceCount(context, newValue);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* ArrayElementAssignment::
generatePrimitiveElementAssignment(IRGenerationContext& context,
                                   const IType* elementType,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

