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
                   IExpression* expression,
                   int line) :
mIsPublic(accessLevel == PUBLIC_ACCESS),
mType(type),
mName(name),
mExpression(expression),
mLine(line) { }

Constant::~Constant() { }

bool Constant::isPublic() const {
  return mIsPublic;
}

string Constant::getName() const {
  return mName;
}

const IType* Constant::getType() const {
  return mType;
}

void Constant::printToStream(IRGenerationContext& context, iostream& stream) const {
  if (!mIsPublic) {
    return;
  }
  
  stream << "  constant " << mType->getTypeName() << " " << mName << ";\n";
}

llvm::Value* Constant::define(IRGenerationContext& context, const IObjectType* objectType) const {
  assert(mExpression && "Constant definition does not have an initialization expression");

  llvm::Type* llvmType = mType->getLLVMType(context);
  llvm::GlobalValue::LinkageTypes linkageType = mIsPublic
  ? llvm::GlobalValue::ExternalLinkage
  : llvm::GlobalValue::InternalLinkage;
  
  if (!mExpression->isConstant()) {
    context.reportError(mLine, "Constant is initialized with a non-constant expression");
    throw 1;
  }
  
  const IType* expressionType = mExpression->getType(context);
  if (expressionType != mType) {
    context.reportError(mLine, "Constant value type does not match declared constant type");
    throw 1;
  }
  
  return new llvm::GlobalVariable(*context.getModule(),
                                  llvmType,
                                  true,
                                  linkageType,
                                  (llvm::Constant*) mExpression->generateIR(context, mType),
                                  getConstantGlobalName(objectType));
}

llvm::Value* Constant::declare(IRGenerationContext& context, const IObjectType* objectType) const {
  llvm::Type* llvmType = mType->getLLVMType(context);
  llvm::GlobalValue::LinkageTypes linkageType = mIsPublic
  ? llvm::GlobalValue::ExternalLinkage
  : llvm::GlobalValue::InternalLinkage;
  
  assert(mExpression == NULL && "External constant is supposed to have no expression");
  
  return new llvm::GlobalVariable(*context.getModule(),
                                  llvmType,
                                  true,
                                  linkageType,
                                  NULL,
                                  getConstantGlobalName(objectType));
  }

string Constant::getConstantGlobalName(const IObjectType* objectType) const {
  return "constant." + objectType->getTypeName() + "." + mName;
}

int Constant::getLine() const {
  return mLine;
}

bool Constant::isConstant() const {
  return true;
}

bool Constant::isField() const {
  return false;
}

bool Constant::isMethod() const {
  return false;
}

bool Constant::isStaticMethod() const {
  return false;
}

bool Constant::isMethodSignature() const {
  return false;
}

bool Constant::isLLVMFunction() const {
  return false;
}
