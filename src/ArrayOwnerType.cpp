//
//  ArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerType::ArrayOwnerType(const ArrayType* arrayType) : mArrayType(arrayType) {
}

ArrayOwnerType::~ArrayOwnerType() {
}

const ArrayType* ArrayOwnerType::getArrayType() const {
  return mArrayType;
}

string ArrayOwnerType::getTypeName() const {
  return mArrayType->getTypeName() + "*";
}

llvm::StructType* ArrayOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArrayType->getLLVMType(context);
}

TypeKind ArrayOwnerType::getTypeKind() const {
  return ARRAY_OWNER_TYPE;
}

bool ArrayOwnerType::canCastTo(const IType* toType) const {
  if (toType == this || toType == mArrayType) {
    return true;
  }
  
  return false;
}

bool ArrayOwnerType::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

llvm::Value* ArrayOwnerType::castTo(IRGenerationContext &context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this || toType == mArrayType) {
    return fromValue;
  }
  
  return NULL;
}

const IType* ArrayOwnerType::getBaseType() const {
  return mArrayType->getBaseType();
}

unsigned long ArrayOwnerType::getSize() const {
  return mArrayType->getSize();
}

const IType* ArrayOwnerType::getScalarType() const {
  return mArrayType->getScalarType();
}

void ArrayOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  if (context.isDestructorDebugOn()) {
    vector<IExpression*> printOutArguments;
    StringLiteral* stringLiteral = new StringLiteral("destructor " + mArrayType->getTypeName() +
                                                     "*\n");
    printOutArguments.push_back(stringLiteral);
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }
  mArrayType->free(context, arrayPointer);
}

void ArrayOwnerType::decrementReferenceCount(IRGenerationContext& context,
                                             llvm::Value* arrayPointer) const {
  mArrayType->decrementReferenceCount(context, arrayPointer);
}
