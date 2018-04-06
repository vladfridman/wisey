//
//  ConstantReference.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Constant.hpp"
#include "wisey/ConstantReference.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ConstantReference::ConstantReference(const IObjectTypeSpecifier* objectTypeSpecifier,
                                     string constantName) :
mObjectTypeSpecifier(objectTypeSpecifier),
mConstantName(constantName) { }

ConstantReference::~ConstantReference() {
  if (mObjectTypeSpecifier != NULL) {
    delete mObjectTypeSpecifier;
  }
}

IVariable* ConstantReference::getVariable(IRGenerationContext& context,
                                          vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* ConstantReference::generateIR(IRGenerationContext& context,
                                     const IType* assignToType) const {
  const IObjectType* objectType = getObjectType(context);
  Constant* constant = objectType->findConstant(mConstantName);
  if (constant->getAccessLevel() == PRIVATE_ACCESS && objectType != context.getObjectType()) {
    Log::e_deprecated("Trying to reference private constant not visible from the current object");
    exit(1);
  }
  string constantGlobalName = constant->getConstantGlobalName(objectType);
  llvm::Constant* constantStore = context.getModule()->getNamedGlobal(constantGlobalName);
  return IRWriter::newLoadInst(context, constantStore, "");
}

const IType* ConstantReference::getType(IRGenerationContext& context) const {
  const IObjectType* objectType = getObjectType(context);
  return objectType->findConstant(mConstantName)->getType();
}

bool ConstantReference::isConstant() const {
  return true;
}

void ConstantReference::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  const IObjectType* objectType = getObjectType(context);
  if (objectType) {
    stream << objectType->getTypeName() << ".";
  }
  stream << mConstantName;
}

const IObjectType* ConstantReference::getObjectType(IRGenerationContext& context) const {
  if (mObjectTypeSpecifier != NULL) {
    return mObjectTypeSpecifier->getType(context);
  }
  
  return context.getObjectType();
}
