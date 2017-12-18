//
//  NullExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NullExpression::NullExpression() { }

NullExpression::~NullExpression() { }

IVariable* NullExpression::getVariable(IRGenerationContext& context,
                                       vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

llvm::Constant* NullExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  return ConstantExpr::getNullValue(getType(context)->getLLVMType(context));
}

const IType* NullExpression::getType(IRGenerationContext& context) const {
  return NullType::NULL_TYPE;
}

bool NullExpression::isConstant() const {
  return true;
}

void NullExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "null";
}
