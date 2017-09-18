//
//  PrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintOutStatement::~PrintOutStatement() {
  for (IExpression* expression : mExpressionList) {
    delete expression;
  }
  mExpressionList.clear();
}

Value* PrintOutStatement::generateIR(IRGenerationContext& context) const {
  string formatString = "";
  for (IExpression* expression : mExpressionList) {
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
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, formatString);
  GlobalVariable* globalVariableString =
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     stringConstant,
                     ".format.str");
  
  Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(llvmContext));
  Constant* indices[] = {zero, zero};
  
  Value* formatPointer = ConstantExpr::getGetElementPtr(NULL, globalVariableString, indices, true);

  Function* printf = IntrinsicFunctions::getPrintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(formatPointer);
  for (IExpression* expression : mExpressionList) {
    arguments.push_back(expression->generateIR(context));
  }
  return IRWriter::createCallInst(context, printf, arguments, "");
}
