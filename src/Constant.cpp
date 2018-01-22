//
//  Constant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Constant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

Constant::Constant(const AccessLevel accessLevel,
                   const IType* type,
                   std::string name,
                   IExpression* expression) :
mAccessLevel(accessLevel),
mType(type),
mName(name),
mExpression(expression) { }

Constant::~Constant() { }

AccessLevel Constant::getAccessLevel() const {
  return mAccessLevel;
}

string Constant::getName() const {
  return mName;
}

const IType* Constant::getType() const {
  return mType;
}

ObjectElementType Constant::getObjectElementType() const {
  return OBJECT_ELEMENT_CONSTANT;
}

void Constant::printToStream(IRGenerationContext& context, iostream& stream) const {
  if (mAccessLevel == PRIVATE_ACCESS) {
    return;
  }
  
  stream << "  public constant " << mType->getTypeName() << " " << mName << " = ";
  mExpression->printToStream(context, stream);
  stream << ";\n";
}

llvm::Value* Constant::generateIR(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  if (!mExpression->isConstant()) {
    Log::e("Constant is initialized with a non-constant expression");
    exit(1);
  }
  
  const IType* type = mExpression->getType(context);
  llvm::Type* llvmType = type->getLLVMType(context);
  llvm::GlobalValue::LinkageTypes linkageType = mAccessLevel == PUBLIC_ACCESS
    ? llvm::GlobalValue::ExternalLinkage
    : llvm::GlobalValue::InternalLinkage;
  
  llvm::Constant* expressionValue = (llvm::Constant*) mExpression->generateIR(context, mType);

  return new llvm::GlobalVariable(*context.getModule(),
                                  llvmType,
                                  true,
                                  linkageType,
                                  expressionValue,
                                  getConstantGlobalName(objectType));
}

string Constant::getConstantGlobalName(const IObjectType* objectType) const {
  return "constant." + objectType->getTypeName() + "." + mName;
}
