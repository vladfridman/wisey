//
//  ArraySpecificType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArraySpecificOwnerType.hpp"
#include "ArraySpecificType.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

ArraySpecificType::ArraySpecificType(const IType* elementType,
                                     list<const IExpression*> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  mArraySpecificOwnerType = new ArraySpecificOwnerType(this);
}

ArraySpecificType::~ArraySpecificType() {
  delete mArraySpecificOwnerType;
}

const ArraySpecificOwnerType* ArraySpecificType::getOwner() const {
  return mArraySpecificOwnerType;
}

const IType* ArraySpecificType::getElementType() const {
  return mElementType;
}

string ArraySpecificType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned i = 0; i < mDimensions.size(); i++) {
    name = name + "[]";
  }
  return name;
}

llvm::PointerType* ArraySpecificType::getLLVMType(IRGenerationContext& context) const {
  return getArrayType(context, 0)->getLLVMType(context);
}

list<tuple<llvm::Value*, llvm::Value*>>
ArraySpecificType::computeArrayAllocData(IRGenerationContext& context, int line) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Type* int64Type = llvm::Type::getInt64Ty(llvmContext);
  llvm::Value* headerSize = llvm::ConstantInt::get(int64Type, 8 * 3);
  list<tuple<llvm::Value*, llvm::Value*>> result;
  
  llvm::Value* elementSize = llvm::ConstantExpr::getSizeOf(mElementType->getLLVMType(context));
  llvm::Value* sizeStore = IRWriter::newAllocaInst(context, int64Type, "arraySize");
  IRWriter::newStoreInst(context, elementSize, sizeStore);
  list<const IExpression*> dimensionsReversed = mDimensions;
  dimensionsReversed.reverse();
  for (const IExpression* dimension : dimensionsReversed) {
    llvm::Value* dimensionValue = dimension->generateIR(context, PrimitiveTypes::VOID);
    const IType* dimensionType = dimension->getType(context);
    checkDimensionType(context, dimensionType, line);
    llvm::Value* dimensionCast = dimensionType->castTo(context,
                                                       dimensionValue,
                                                       PrimitiveTypes::LONG,
                                                       0);
    llvm::Value* sizeValue = IRWriter::newLoadInst(context, sizeStore, "size");
    result.push_front(make_tuple(dimensionCast, sizeValue));
    llvm::Value* newSize = IRWriter::createBinaryOperator(context,
                                                          llvm::Instruction::Mul,
                                                          dimensionCast,
                                                          sizeValue,
                                                          "");
    llvm::Value* withHeader = IRWriter::createBinaryOperator(context,
                                                             llvm::Instruction::Add,
                                                             newSize,
                                                             headerSize,
                                                             "");
    IRWriter::newStoreInst(context, withHeader, sizeStore);
  }
  llvm::Value* arraySize = IRWriter::newLoadInst(context, sizeStore, "");
  result.push_front(make_tuple(llvm::ConstantInt::get(int64Type, 1), arraySize));
  return result;
}

bool ArraySpecificType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

bool ArraySpecificType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

llvm::Value* ArraySpecificType::castTo(IRGenerationContext &context,
                                       llvm::Value* fromValue,
                                       const IType* toType,
                                       int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

unsigned long ArraySpecificType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool ArraySpecificType::isPrimitive() const {
  return false;
}

bool ArraySpecificType::isOwner() const {
  return false;
}

bool ArraySpecificType::isReference() const {
  return true;
}

bool ArraySpecificType::isArray() const {
  return true;
}

bool ArraySpecificType::isFunction() const {
  return false;
}

bool ArraySpecificType::isPackage() const {
  return false;
}

bool ArraySpecificType::isController() const {
  return false;
}

bool ArraySpecificType::isInterface() const {
  return false;
}

bool ArraySpecificType::isModel() const {
  return false;
}

bool ArraySpecificType::isNode() const {
  return false;
}

bool ArraySpecificType::isNative() const {
  return false;
}

bool ArraySpecificType::isPointer() const {
  return false;
}

bool ArraySpecificType::isImmutable() const {
  return false;
}

void ArraySpecificType::checkDimensionType(IRGenerationContext& context,
                                           const IType* type,
                                           int line) const {
  if (type->canAutoCastTo(context, PrimitiveTypes::LONG)) {
    return;
  }
  context.reportError(line, "Dimension in array allocation should be castable to long, "
                      "but it is of " + type->getTypeName() + " type");
  throw 1;
}

void ArraySpecificType::printToStream(IRGenerationContext &context, iostream& stream) const {
  mElementType->printToStream(context, stream);
  for (const IExpression* dimension : mDimensions) {
    stream << "[";
    dimension->printToStream(context, stream);
    stream << "]";
  }
}

void ArraySpecificType::createLocalVariable(IRGenerationContext& context,
                                            string name,
                                            int line) const {
  assert(false);
}

void ArraySpecificType::createFieldVariable(IRGenerationContext& context,
                                            string name,
                                            const IConcreteObjectType* object,
                                            int line) const {
  assert(false);
}

void ArraySpecificType::createParameterVariable(IRGenerationContext& context,
                                                string name,
                                                llvm::Value* value,
                                                int line) const {
  assert(false);
}

const wisey::ArrayType* ArraySpecificType::getArrayType(IRGenerationContext& context, int line) const {
  return context.getArrayType(mElementType, getNumberOfDimensions());
}

llvm::Instruction* ArraySpecificType::inject(IRGenerationContext& context,
                                             const InjectionArgumentList injectionArgumentList,
                                             int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
