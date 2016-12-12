//
//  RelationalExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalExpression_h
#define RelationalExpression_h

#include <llvm/IR/Value.h>

#include "yazyk/codegen.hpp"
#include "yazyk/node.hpp"

namespace yazyk {
  
/**
 * Represents a relational expression such as a >= b
 */
class RelationalExpression : public IExpression {
  IExpression& lhs;
  IExpression& rhs;
  RelationalOperation operation;
  
public:

  RelationalExpression(IExpression& lhs, RelationalOperation operation, IExpression& rhs) :
    lhs(lhs), rhs(rhs), operation(operation) { }

  ~RelationalExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
}

#endif /* RelationalExpression_h */
