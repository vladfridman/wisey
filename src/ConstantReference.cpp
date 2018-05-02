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
                                     string constantName,
                                     int line) :
mObjectTypeSpecifier(objectTypeSpecifier),
mConstantName(constantName),
mLine(line) { }

ConstantReference::~ConstantReference() {
  if (mObjectTypeSpecifier != NULL) {
    delete mObjectTypeSpecifier;
  }
}

int ConstantReference::getLine() const {
  return mLine;
}

Value* ConstantReference::generateIR(IRGenerationContext& context,
                                     const IType* assignToType) const {
  const IObjectType* objectType = getObjectType(context);
  Constant* constant = objectType->findConstant(mConstantName);
  if (!constant->isPublic() && objectType != context.getObjectType()) {
    context.reportError(mLine,
                        "Trying to reference private constant not visible from the current object");
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

bool ConstantReference::isAssignable() const {
  return false;
}

void ConstantReference::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  if (mObjectTypeSpecifier) {
    mObjectTypeSpecifier->printToStream(context, stream);
    stream << ".";
  }
  stream << mConstantName;
}

const IObjectType* ConstantReference::getObjectType(IRGenerationContext& context) const {
  if (mObjectTypeSpecifier != NULL) {
    return mObjectTypeSpecifier->getType(context);
  }
  
  return context.getObjectType();
}
