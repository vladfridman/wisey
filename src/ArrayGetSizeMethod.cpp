//
//  ArrayGetSizeMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "ArrayGetSizeMethod.hpp"
#include "Composer.hpp"
#include "IExpression.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ArrayGetSizeMethod::ArrayGetSizeMethod() : mMethodQualifiers(new MethodQualifiers(0)) {
}

ArrayGetSizeMethod::~ArrayGetSizeMethod() {
  delete mMethodQualifiers;
}

const ArrayGetSizeMethod* ArrayGetSizeMethod::ARRAY_GET_SIZE_METHOD = new ArrayGetSizeMethod();

const std::string ArrayGetSizeMethod::ARRAY_GET_SIZE_METHOD_NAME = "getSize";

Value* ArrayGetSizeMethod::generateIR(IRGenerationContext& context,
                                      const IExpression* expression,
                                      int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* arrayValue = expression->generateIR(context, PrimitiveTypes::VOID);
  Composer::checkForNull(context, arrayValue, line);
  // Array layout: { i64 refcount, i64 size, i64, [0 x T] }. The size field
  // sits at byte offset 8 from the array pointer.
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 8);
  Value* sizeStore = IRWriter::createGetElementPtrInst(context,
                                                       Type::getInt8Ty(llvmContext),
                                                       arrayValue,
                                                       index);
  return IRWriter::newLoadInst(context,
                               Type::getInt64Ty(llvmContext),
                               sizeStore,
                               "");
}

string ArrayGetSizeMethod::getName() const {
  return ARRAY_GET_SIZE_METHOD_NAME;
}

bool ArrayGetSizeMethod::isPublic() const {
  return true;
}

const IType* ArrayGetSizeMethod::getReturnType() const {
  return PrimitiveTypes::LONG;
}

vector<const wisey::Argument*> ArrayGetSizeMethod::getArguments() const {
  vector<const wisey::Argument*> emptyList;
  return emptyList;
}

vector<const Model*> ArrayGetSizeMethod::getThrownExceptions() const {
  vector<const Model*> emptyList;
  return emptyList;
}

bool ArrayGetSizeMethod::isStatic() const {
  return false;
}

bool ArrayGetSizeMethod::isOverride() const {
  return false;
}

FunctionType* ArrayGetSizeMethod::getLLVMType(IRGenerationContext& context) const {
  return FunctionType::get(PrimitiveTypes::LONG->getLLVMType(context), false);
}

const IObjectType* ArrayGetSizeMethod::getParentObject() const {
  return NULL;
}

MethodQualifiers* ArrayGetSizeMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string ArrayGetSizeMethod::getTypeName() const {
  return "[]." + ARRAY_GET_SIZE_METHOD_NAME;
}

bool ArrayGetSizeMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool ArrayGetSizeMethod::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

Value* ArrayGetSizeMethod::castTo(IRGenerationContext& context,
                                  Value* fromValue,
                                  const IType* toType,
                                  int line) const {
  assert(false && "Attempting to cast array getSize() method");
}

bool ArrayGetSizeMethod::isPrimitive() const {
  return false;
}

bool ArrayGetSizeMethod::isOwner() const {
  return false;
}

bool ArrayGetSizeMethod::isReference() const {
  return false;
}

bool ArrayGetSizeMethod::isArray() const {
  return false;
}

bool ArrayGetSizeMethod::isFunction() const {
  return true;
}

bool ArrayGetSizeMethod::isPackage() const {
  return false;
}

bool ArrayGetSizeMethod::isController() const {
  return false;
}

bool ArrayGetSizeMethod::isInterface() const {
  return false;
}

bool ArrayGetSizeMethod::isModel() const {
  return false;
}

bool ArrayGetSizeMethod::isNode() const {
  return false;
}

bool ArrayGetSizeMethod::isNative() const {
  return false;
}

bool ArrayGetSizeMethod::isPointer() const {
  return false;
}

bool ArrayGetSizeMethod::isImmutable() const {
  return false;
}

void ArrayGetSizeMethod::createLocalVariable(IRGenerationContext& context,
                                             std::string name,
                                             int line) const {
  assert(false && "Tring to create a variable of method type");
}

void ArrayGetSizeMethod::createFieldVariable(IRGenerationContext& context,
                                             std::string name,
                                             const IConcreteObjectType* object,
                                             int line) const {
  assert(false && "Tring to create a variable of method type");
}

void ArrayGetSizeMethod::createParameterVariable(IRGenerationContext& context,
                                                 std::string name,
                                                 llvm::Value* value,
                                                 int line) const {
  assert(false && "Tring to create a variable of method type");
}

const wisey::ArrayType* ArrayGetSizeMethod::getArrayType(IRGenerationContext& context,
                                                         int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Value* ArrayGetSizeMethod::inject(IRGenerationContext& context,
                                  const InjectionArgumentList injectionArgumentList,
                                  int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

void ArrayGetSizeMethod:: printToStream(IRGenerationContext& context,
                                        iostream& stream) const {
  assert(false && "Attempt to print array getSize() method");
}
