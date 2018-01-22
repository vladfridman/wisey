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
  IExpression* mExpression;
  long long mIncrementBy;
  std::string mVariableName;
  bool mIsPrefix;
  int mLine;
  
  IncrementExpression(IExpression* expression,
                      long long incrementBy,
                      std::string variableName,
                      bool isPrefix,
                      int line);
  
  ~IncrementExpression();
  
public:
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;

  llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  /**
   * Create increment by one expression
   */
  static IncrementExpression* newIncrementByOne(IExpression* expression, int line);
  
  /**
   * Create decrement by one expression
   */
  static IncrementExpression* newDecrementByOne(IExpression* expression, int line);
  
};
  
} /* namespace wisey */

#endif /* IncrementExpression_h */
