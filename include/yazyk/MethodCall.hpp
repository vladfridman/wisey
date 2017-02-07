//
//  MethodCall.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodCall_h
#define MethodCall_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a method call identified by id and given arguments
 */
class MethodCall : public IExpression {
  const Identifier& mId;
  ExpressionList mArguments;
  
public:
  MethodCall(const Identifier& id, ExpressionList& arguments) :
    mId(id), mArguments(arguments) { }
  MethodCall(const Identifier& id) : mId(id) { }
    ~MethodCall() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;

private:
  llvm::Function* declarePrintf(IRGenerationContext& context) const;
};
  
} /* namespace yazyk */

#endif /* MethodCall_h */
