//
//  LongConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LongConstant.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* LongConstant::getVariable(IRGenerationContext& context,
                                     vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

llvm::Constant* LongConstant::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}

const IType* LongConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::LONG_TYPE;
}

bool LongConstant::isConstant() const {
  return true;
}

void LongConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue << "l";
}
