//
//  ArrayAllocation.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ArrayAllocation::ArrayAllocation(const ArrayTypeSpecifier* arrayTypeSpecifier) :
mArrayTypeSpecifier(arrayTypeSpecifier) {
}

ArrayAllocation::~ArrayAllocation() {
  delete mArrayTypeSpecifier;
}

Value* ArrayAllocation::generateIR(IRGenerationContext &context, IRGenerationFlag flag) const {
  ArrayType* arrayType = mArrayTypeSpecifier->getType(context);
  
  Type* structType = arrayType->getLLVMType(context);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "newarray");
  IntrinsicFunctions::setMemoryToZero(context, malloc, structType);
  
  if (flag == IR_GENERATION_RELEASE) {
    return malloc;
  }

  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);
  
  IVariable* heapVariable = NULL;
  
  const IType* elementType = arrayType->getScalarType();
  assert(elementType->getTypeKind() == PRIMITIVE_TYPE);
  heapVariable = new LocalArrayOwnerVariable(IVariable::getTemporaryVariableName(this),
                                             arrayType->getOwner(),
                                             alloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

IVariable* ArrayAllocation::getVariable(IRGenerationContext &context,
                                        vector<const IExpression *> &arrayIndices) const {
  return NULL;
}

bool ArrayAllocation::isConstant() const {
  return false;
}

const IType* ArrayAllocation::getType(IRGenerationContext& context) const {
  return mArrayTypeSpecifier->getType(context)->getOwner();
}

void ArrayAllocation::printToStream(IRGenerationContext &context, iostream &stream) const {
  stream << "new ";
  mArrayTypeSpecifier->printToStream(context, stream);
}
