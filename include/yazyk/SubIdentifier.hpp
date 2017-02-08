//
//  SubIdentifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef SubIdentifier_h
#define SubIdentifier_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents an identifier that refers to model's field or a method
 */
class SubIdentifier : public IExpression {
  
  IExpression& mExpression;
  std::string mIdentifier;
  
public:
  
  SubIdentifier(IExpression& expression, std::string identifier)
    : mExpression(expression), mIdentifier(identifier) { }
  
  ~SubIdentifier() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;

private:
  
  ModelField* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* SubIdentifier_h */
