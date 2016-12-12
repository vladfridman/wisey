//
//  AddditiveMultiplicativeExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef AddditiveMultiplicativeExpression_hpp
#define AddditiveMultiplicativeExpression_hpp

#include <llvm/IR/Value.h>

#include "yazyk/codegen.hpp"
#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents simple binary expression such as addition or division
 */
class AddditiveMultiplicativeExpression : public IExpression {
  IExpression& lhs;
  IExpression& rhs;
  int operation;
  
public:
  AddditiveMultiplicativeExpression(IExpression& lhs, int operation, IExpression& rhs) :
    lhs(lhs), rhs(rhs), operation(operation) { }
  
  ~AddditiveMultiplicativeExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

}

#endif /* AddditiveMultiplicativeExpression_hpp */
