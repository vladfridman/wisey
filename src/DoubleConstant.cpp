//
//  DoubleConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DoubleConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* DoubleConstant::getVariable(IRGenerationContext& context,
                                       vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

llvm::Constant* DoubleConstant::generateIR(IRGenerationContext& context,
                                           IRGenerationFlag flag) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}

const IType* DoubleConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::DOUBLE_TYPE;
}

bool DoubleConstant::isConstant() const {
  return true;
}

void DoubleConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue << "d";
}
