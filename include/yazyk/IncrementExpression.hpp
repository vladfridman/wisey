//
//  IncrementExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IncrementExpression_h
#define IncrementExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/Identifier.hpp"

namespace yazyk {

/**
 * Represents an increment or decrement expresssion such as i++ or i--
 */
class IncrementExpression : public IExpression {
  Identifier identifier;
  long long incrementBy;
  std::string variableName;
  bool isPrefix;
  
  IncrementExpression(Identifier &identifier,
                      long long incrementBy,
                      std::string variableName,
                      bool isPrefix) :
    identifier(identifier),
    incrementBy(incrementBy),
    variableName(variableName),
    isPrefix(isPrefix) { }
  
  ~IncrementExpression() { }
  
public:
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  /**
   * Create increment by one expression
   */
  static IncrementExpression * newIncrementByOne(Identifier &identifier) {
    return new IncrementExpression(identifier, 1, "inc", false);
  }
  
  /**
   * Create decrement by one expression
   */
  static IncrementExpression * newDecrementByOne(Identifier &identifier) {
    return new IncrementExpression(identifier, -1, "dec", false);
  }
};
  
} /* namespace yazyk */

#endif /* IncrementExpression_h */
