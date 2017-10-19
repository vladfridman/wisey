//
//  IPrintStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IPrintStatement.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* IPrintStatement::getFormatString(IRGenerationContext& context,
                                        vector<IExpression*> expressionList) {
  string formatString = "";
  for (IExpression* expression : expressionList) {
    const IType* type = expression->getType(context);
    if (type->getTypeKind() != PRIMITIVE_TYPE) {
      Log::e("Can not print non primitive types");
      exit(1);
    }
    if (type == PrimitiveTypes::VOID_TYPE) {
      Log::e("Can not print a void type expression");
      exit(1);
    }
    const IPrimitiveType* primitiveType = (const IPrimitiveType*) type;
    formatString += primitiveType->getFormat();
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, formatString);
  GlobalVariable* globalVariableString =
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     stringConstant,
                     ".format.str");
  
  llvm::Constant* zero = llvm::Constant::getNullValue(IntegerType::getInt32Ty(llvmContext));
  llvm::Constant* indices[] = {zero, zero};
  
  return ConstantExpr::getGetElementPtr(NULL, globalVariableString, indices, true);
}
