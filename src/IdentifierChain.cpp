//
//  IdentifierChain.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/IMethodCall.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/Log.hpp"
#include "wisey/UndefinedType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

IdentifierChain::IdentifierChain(IExpression* objectExpression, const std::string name) :
mObjectExpression(objectExpression), mName(name) {
}

IdentifierChain::~IdentifierChain() {
  delete mObjectExpression;
}

IVariable* IdentifierChain::getVariable(IRGenerationContext& context,
                                        vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* IdentifierChain::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectWithMethodsType = methodDescriptor->getParentObject();
  if (!checkAccess(context, methodDescriptor)) {
    Log::e("Method '" + mName + "' of object " + objectWithMethodsType->getTypeName() +
           " is private");
    exit(1);
  }

  return mObjectExpression->generateIR(context, assignToType);
}

const IType* IdentifierChain::getType(IRGenerationContext& context) const {
  const IType* expressionType = mObjectExpression->getType(context);
  if (expressionType->isPackage() || expressionType == UndefinedType::UNDEFINED_TYPE) {
    stringstream stringStream;
    mObjectExpression->printToStream(context, stringStream);
    return context.getPackageType(stringStream.str() + "." + mName);
  }
  
  return getMethodDescriptor(context);
}

bool IdentifierChain::isConstant() const {
  return false;
}

void IdentifierChain::printToStream(IRGenerationContext& context, iostream& stream) const {
  mObjectExpression->printToStream(context, stream);
  stream << "." << mName;
}

IMethodDescriptor* IdentifierChain::getMethodDescriptor(IRGenerationContext& context) const {
  const IType* expressionType = mObjectExpression->getType(context);
  if (!IType::isObjectType(expressionType)) {
    Log::e("Attempt to call a method '" + mName + "' on an identifier that is not an object");
    exit(1);
  }
  
  const IObjectType* objectType = expressionType->isOwner()
  ? (const IObjectType*) ((const IObjectOwnerType*) expressionType)->getReference()
  : (const IObjectType*) expressionType;
  
  IMethodDescriptor* methodDescriptor = objectType->findMethod(mName);
  
  if (methodDescriptor == NULL) {
    Log::e("Method '" + mName + "' is not found in object " + objectType->getTypeName());
    exit(1);
  }
  
  assert(objectType == methodDescriptor->getParentObject());

  return methodDescriptor;
}

bool IdentifierChain::checkAccess(IRGenerationContext& context,
                                  IMethodDescriptor* methodDescriptor) const {

  if (methodDescriptor->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  IVariable* thisVariable = context.getThis();
  if (thisVariable == NULL) {
    return context.getObjectType() == methodDescriptor->getParentObject();
  }
  
  return thisVariable != NULL && thisVariable->getType() == methodDescriptor->getParentObject();
}
