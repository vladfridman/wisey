//
//  StringGetLength.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringGetLengthMethod.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

StringGetLengthMethod::StringGetLengthMethod() : mMethodQualifiers(new MethodQualifiers(0)) {
}

StringGetLengthMethod::~StringGetLengthMethod() {
  delete mMethodQualifiers;
}

const StringGetLengthMethod* StringGetLengthMethod::STRING_GET_LENGTH_METHOD =
new StringGetLengthMethod();

const std::string StringGetLengthMethod::STRING_GET_LENGTH_METHOD_NAME = "getLength";

Value* StringGetLengthMethod::generateIR(IRGenerationContext& context,
                                         const IExpression* expression) {
  Value* expressionValue = expression->generateIR(context, PrimitiveTypes::VOID);
  Function* strlen = IntrinsicFunctions::getStrLenFunction(context);
  vector<Value*> arguments;
  arguments.push_back(expressionValue);
  return IRWriter::createCallInst(context, strlen, arguments, "");
}

string StringGetLengthMethod::getName() const {
  return STRING_GET_LENGTH_METHOD_NAME;
}

bool StringGetLengthMethod::isPublic() const {
  return true;
}

const IType* StringGetLengthMethod::getReturnType() const {
  return PrimitiveTypes::LONG;
}

vector<const wisey::Argument*> StringGetLengthMethod::getArguments() const {
  vector<const wisey::Argument*> emptyList;
  return emptyList;
}

vector<const Model*> StringGetLengthMethod::getThrownExceptions() const {
  vector<const Model*> emptyList;
  return emptyList;
}

bool StringGetLengthMethod::isStatic() const {
  return false;
}

bool StringGetLengthMethod::isOverride() const {
  return false;
}

FunctionType* StringGetLengthMethod::getLLVMType(IRGenerationContext& context) const {
  return FunctionType::get(PrimitiveTypes::LONG->getLLVMType(context), false);
}

const IObjectType* StringGetLengthMethod::getParentObject() const {
  return NULL;
}

MethodQualifiers* StringGetLengthMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string StringGetLengthMethod::getTypeName() const {
  return "string." + STRING_GET_LENGTH_METHOD_NAME;
}

bool StringGetLengthMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool StringGetLengthMethod::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

Value* StringGetLengthMethod::castTo(IRGenerationContext& context,
                                  Value* fromValue,
                                  const IType* toType,
                                  int line) const {
  assert(false && "Attempting to cast array getSize() method");
}

bool StringGetLengthMethod::isPrimitive() const {
  return false;
}

bool StringGetLengthMethod::isOwner() const {
  return false;
}

bool StringGetLengthMethod::isReference() const {
  return false;
}

bool StringGetLengthMethod::isArray() const {
  return false;
}

bool StringGetLengthMethod::isFunction() const {
  return true;
}

bool StringGetLengthMethod::isPackage() const {
  return false;
}

bool StringGetLengthMethod::isController() const {
  return false;
}

bool StringGetLengthMethod::isInterface() const {
  return false;
}

bool StringGetLengthMethod::isModel() const {
  return false;
}

bool StringGetLengthMethod::isNode() const {
  return false;
}

bool StringGetLengthMethod::isNative() const {
  return false;
}

bool StringGetLengthMethod::isPointer() const {
  return false;
}

bool StringGetLengthMethod::isImmutable() const {
  return false;
}

void StringGetLengthMethod::createLocalVariable(IRGenerationContext& context,
                                             std::string name,
                                             int line) const {
  assert(false && "Tring to create a variable of method type");
}

void StringGetLengthMethod::createFieldVariable(IRGenerationContext& context,
                                             std::string name,
                                             const IConcreteObjectType* object,
                                             int line) const {
  assert(false && "Tring to create a variable of method type");
}

void StringGetLengthMethod::createParameterVariable(IRGenerationContext& context,
                                                 std::string name,
                                                 llvm::Value* value,
                                                 int line) const {
  assert(false && "Tring to create a variable of method type");
}

const wisey::ArrayType* StringGetLengthMethod::getArrayType(IRGenerationContext& context,
                                                         int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Value* StringGetLengthMethod::inject(IRGenerationContext& context,
                                  const InjectionArgumentList injectionArgumentList,
                                  int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

void StringGetLengthMethod:: printToStream(IRGenerationContext& context,
                                        iostream& stream) const {
  assert(false && "Attempt to print array getSize() method");
}
