//
//  FunctionCall.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef FunctionCall_h
#define FunctionCall_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a function call identified by id and given arguments
 */
class FunctionCall : public IExpression {
  std::string mFunctionName;
  ExpressionList mArguments;
  
public:
  FunctionCall(std::string functionName, ExpressionList& arguments) :
    mFunctionName(functionName), mArguments(arguments) { }
  
  FunctionCall(std::string functionName) : mFunctionName(functionName) { }

  ~FunctionCall() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;

private:
  
  llvm::Function* declarePrintf(IRGenerationContext& context) const;
};
  
} /* namespace yazyk */

#endif /* FunctionCall_h */
