//
//  IncrementExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IncrementExpression_h
#define IncrementExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {

/**
 * Represents an increment or decrement expresssion such as i++ or i--
 */
class IncrementExpression : public IExpression {
  Identifier* mIdentifier;
  long long mIncrementBy;
  std::string mVariableName;
  bool mIsPrefix;
  
  IncrementExpression(Identifier* identifier,
                      long long incrementBy,
                      std::string variableName,
                      bool isPrefix) :
    mIdentifier(identifier),
    mIncrementBy(incrementBy),
    mVariableName(variableName),
    mIsPrefix(isPrefix) { }
  
  ~IncrementExpression();
  
public:
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  /**
   * Create increment by one expression
   */
  static IncrementExpression * newIncrementByOne(Identifier* identifier) {
    return new IncrementExpression(identifier, 1, "inc", false);
  }
  
  /**
   * Create decrement by one expression
   */
  static IncrementExpression * newDecrementByOne(Identifier* identifier) {
    return new IncrementExpression(identifier, -1, "dec", false);
  }
};
  
} /* namespace wisey */

#endif /* IncrementExpression_h */
