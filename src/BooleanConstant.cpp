//
//  BooleanConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/BooleanConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* BooleanConstant::getVariable(IRGenerationContext& context,
                                        vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

llvm::Constant* BooleanConstant::generateIR(IRGenerationContext& context,
                                            IRGenerationFlag flag) const {
  return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), mValue);
}

IType* BooleanConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

bool BooleanConstant::isConstant() const {
  return true;
}

void BooleanConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  if (mValue) {
    stream << "true";
  } else {
    stream << "false";
  }
}
