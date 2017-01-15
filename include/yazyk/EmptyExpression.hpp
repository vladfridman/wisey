//
//  EmptyExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyExpression_h
#define EmptyExpression_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents an empty expression that does not do anything
 */
class EmptyExpression : public IExpression {

public:

  const static EmptyExpression EMPTY_EXPRESSION;

  EmptyExpression() { }
  
  ~EmptyExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* EmptyExpression_h */