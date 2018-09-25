//
//  StoreInReference.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"
#include "StoreInReference.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

StoreInReference::StoreInReference(IExpression* valueExpression,
                                   IExpression* pointerExpression,
                                   int line) :
mValueExpression(valueExpression), mPointerExpression(pointerExpression), mLine(line) {
  assert(mValueExpression && "Value expression is null in StoreInReference");
  assert(mPointerExpression && "Pointer expression is null in StoreInReference");
}

StoreInReference::~StoreInReference() {
  delete mValueExpression;
  delete mPointerExpression;
}

void StoreInReference::generateIR(IRGenerationContext& context) const {
  const IType* valueType = mValueExpression->getType(context);
  const IType* pointerType = mPointerExpression->getType(context);
  Type* valueLLVMType = valueType->getLLVMType(context);
  Type* pointerLLVMType = pointerType->getLLVMType(context);
  if (!pointerLLVMType->isPointerTy()) {
    context.reportError(mLine, "Second parameter in ::llvm::store is not of pointer type");
    throw 1;
  }
  if (pointerLLVMType->getPointerElementType() != valueLLVMType) {
    context.reportError(mLine, "First parameter in ::llvm::store is not of type that is "
                        "compatable with pointer type of the second parameter");
    throw 1;
  }
  Value* value = mValueExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* pointer = mPointerExpression->generateIR(context, PrimitiveTypes::VOID);
  IRWriter::newStoreInst(context, value, pointer);
}
