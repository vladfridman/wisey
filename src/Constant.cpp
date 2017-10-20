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

ObjectElementType Constant::getObjectElementType() const {
  return OBJECT_ELEMENT_CONSTANT;
}

void Constant::printToStream(IRGenerationContext& context, iostream& stream) const {
  if (mAccessLevel == PRIVATE_ACCESS) {
    return;
  }
  
  stream << "  public constant " << mType->getName() << " " << mName << " = ";
  mExpression->printToStream(context, stream);
  stream << ";\n";
}

llvm::Value* Constant::generateIR(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  if (!mExpression->isConstant()) {
    Log::e("Constant is initialized with a non-constant expression");
    exit(1);
  }
  
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  const IType* type = mExpression->getType(context);
  llvm::Type* llvmType = type->getLLVMType(llvmContext);
  llvm::GlobalValue::LinkageTypes linkageType = mAccessLevel == PUBLIC_ACCESS
    ? llvm::GlobalValue::ExternalLinkage
    : llvm::GlobalValue::InternalLinkage;
  
  return new llvm::GlobalVariable(*context.getModule(),
                                  llvmType,
                                  true,
                                  linkageType,
                                  (llvm::Constant*) mExpression->generateIR(context),
                                  getConstantGlobalName(objectType));
}

string Constant::getConstantGlobalName(const IObjectType* objectType) const {
  return "constant." + objectType->getName() + "." + mName;
}
