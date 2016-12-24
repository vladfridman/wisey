//
//  Assignment.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Assignment_h
#define Assignment_h

#include "yazyk/Identifier.hpp"
#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents an assignment expression
 */
class Assignment : public IExpression {
  Identifier& mIdentifier;
  IExpression& mExpression;
  
public:

  Assignment(Identifier& identifier, IExpression& expression)
    : mIdentifier(identifier), mExpression(expression) { }

  ~Assignment() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
}

#endif /* Assignment_h */
