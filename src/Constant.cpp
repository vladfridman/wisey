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
  
  stream << "  public constant " << mType->getTypeName() << " " << mName << " = ";
  mExpression->printToStream(context, stream);
  stream << ";\n";
}

llvm::Value* Constant::generateIR(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  if (!mExpression->isConstant()) {
    context.reportError(mLine, "Constant is initialized with a non-constant expression");
    throw 1;
  }
  
  const IType* type = mExpression->getType(context);
  llvm::Type* llvmType = type->getLLVMType(context);
  llvm::GlobalValue::LinkageTypes linkageType = mIsPublic
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
