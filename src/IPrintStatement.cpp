//
//  IPrintStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/IPrintStatement.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExpressionList IPrintStatement::getExpressions(IRGenerationContext& context,
                                               const IExpression* expression,
                                               int line) {
  const IType* expressionType = expression->getType(context);
  assert(expressionType->isPrimitive() && "Attempting to printa nonprimitive type");
  assert(expressionType != PrimitiveTypes::VOID &&
         "Attempting to print a void type expression");
  
  ExpressionList expressions;
  if (expressionType != PrimitiveTypes::STRING_FORMAT) {
    expressions.push_back(expression);
    return expressions;
  }
  
  const AdditiveMultiplicativeExpression* addition =
    (const AdditiveMultiplicativeExpression*) expression;
  const IExpression* leftExpression = addition->getLeft();
  const IExpression* rightExpression = addition->getRight();
  ExpressionList leftExpressions = getExpressions(context, leftExpression, line);
  ExpressionList rightExpressions = getExpressions(context, rightExpression, line);
  for (const IExpression* leftExpression : leftExpressions) {
    expressions.push_back(leftExpression);
  }
  for (const IExpression* rightExpression : rightExpressions) {
    expressions.push_back(rightExpression);
  }
  return expressions;
}

Value* IPrintStatement::getFormatString(IRGenerationContext& context,
                                        ExpressionList expressionList,
                                        int line) {
  string formatString = "";
  for (const IExpression* expression : expressionList) {
    const IType* type = expression->getType(context);
    if (!type->isPrimitive()) {
      context.reportError(line, "Can not print non primitive types");
      exit(1);
    }
    if (type == PrimitiveTypes::VOID) {
      context.reportError(line, "Can not print a void type expression");
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
