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
#include "yazyk/Identifier.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a function call identified by id and given arguments
 */
class FunctionCall : public IExpression {
  const Identifier& mId;
  ExpressionList mArguments;
  
public:
  FunctionCall(const Identifier& id, ExpressionList& arguments) : mId(id), mArguments(arguments) { }
  
  FunctionCall(const Identifier& id) : mId(id) { }

  ~FunctionCall() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;

private:
  llvm::Function* declarePrintf(IRGenerationContext& context) const;
};
  
} /* namespace yazyk */

#endif /* FunctionCall_h */
