//
//  EmptyExpressionStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyExpressionStatement_h
#define EmptyExpressionStatement_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents an empty statement that does not do anything
 */
class EmptyExpressionStatement : public IStatement {
  
public:
  EmptyExpressionStatement() { }
  
  ~EmptyExpressionStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override { return NULL; }
};
  
} /* namespace yazyk */

#endif /* EmptyExpressionStatement_h */
