//
//  EmptyStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyStatement_h
#define EmptyStatement_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents an empty statement that does not do anything
 */
class EmptyStatement : public IStatement {
  
public:
  EmptyStatement() { }
  
  ~EmptyStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override { return NULL; }
};
  
} /* namespace yazyk */

#endif /* EmptyStatement_h */
