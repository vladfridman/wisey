//
//  IdentifierChain.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "ArrayGetSizeMethod.hpp"
#include "IRGenerationContext.hpp"
#include "IdentifierChain.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"
#include "StringGetLengthMethod.hpp"
#include "UndefinedType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

IdentifierChain::IdentifierChain(const IExpression* objectExpression, std::string name, int line) :
mObjectExpression(objectExpression), mName(name), mLine(line) {
}

IdentifierChain::~IdentifierChain() {
  delete mObjectExpression;
}

int IdentifierChain::getLine() const {
  return mLine;
}

Value* IdentifierChain::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectWithMethodsType = methodDescriptor->getParentObject();
  if (!checkAccess(context, methodDescriptor)) {
    context.reportError(mLine,
                        "Method '" + mName + "' of object " + objectWithMethodsType->getTypeName() +
                        " is private");
    throw 1;
  }

  return mObjectExpression->generateIR(context, assignToType);
}

const IType* IdentifierChain::getType(IRGenerationContext& context) const {
  const IType* expressionType = mObjectExpression->getType(context);
  if (expressionType->isPackage() || expressionType == UndefinedType::UNDEFINED) {
    stringstream stringStream;
    mObjectExpression->printToStream(context, stringStream);
    return context.getPackageType(stringStream.str() + "." + mName);
  }
  
  return getMethodDescriptor(context);
}

bool IdentifierChain::isConstant() const {
  return false;
}

bool IdentifierChain::isAssignable() const {
  return false;
}

void IdentifierChain::printToStream(IRGenerationContext& context, iostream& stream) const {
  mObjectExpression->printToStream(context, stream);
  stream << "." << mName;
}

const IMethodDescriptor* IdentifierChain::getMethodDescriptor(IRGenerationContext& context) const {
  const IType* expressionType = mObjectExpression->getType(context);
  if (expressionType == UndefinedType::UNDEFINED) {
    context.reportError(mLine, "Attempt to call a method '" + mName +
                        "' on undefined type expression");
    throw 1;
  }
  if (expressionType->isArray() && !mName.compare(ArrayGetSizeMethod::ARRAY_GET_SIZE_METHOD_NAME)) {
    return ArrayGetSizeMethod::ARRAY_GET_SIZE_METHOD;
  }
  if (expressionType == PrimitiveTypes::STRING &&
      !mName.compare(StringGetLengthMethod::STRING_GET_LENGTH_METHOD_NAME)) {
    return StringGetLengthMethod::STRING_GET_LENGTH_METHOD;
  }
  if (!IType::isObjectType(expressionType)) {
    context.reportError(mLine, "Attempt to call a method '" + mName +
                        "' on an expression that is not of object type");
    throw 1;
  }

  const IObjectType* objectType = expressionType->isOwner()
  ? (const IObjectType*) ((const IObjectOwnerType*) expressionType)->getReference()
  : (const IObjectType*) expressionType;
  
  const IMethodDescriptor* methodDescriptor = objectType->findMethod(mName);
  
  if (methodDescriptor == NULL) {
    context.reportError(mLine, "Method '" + mName + "' is not found in object " +
                        objectType->getTypeName());
    throw 1;
  }
  
  assert(objectType == methodDescriptor->getParentObject());

  return methodDescriptor;
}

bool IdentifierChain::checkAccess(IRGenerationContext& context,
                                  const IMethodDescriptor* methodDescriptor) const {

  if (methodDescriptor->isPublic()) {
    return true;
  }
  IVariable* thisVariable = context.getThis();
  if (thisVariable == NULL) {
    return context.getObjectType() == methodDescriptor->getParentObject();
  }
  
  return thisVariable != NULL && thisVariable->getType() == methodDescriptor->getParentObject();
}
