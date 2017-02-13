//
//  ModelFieldReference.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelFieldReference_h
#define ModelFieldReference_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents an identifier that refers to model's field
 */
class ModelFieldReference : public IExpression {
  
  IExpression& mExpression;
  std::string mIdentifier;
  
public:
  
  ModelFieldReference(IExpression& expression, std::string identifier)
    : mExpression(expression), mIdentifier(identifier) { }
  
  ~ModelFieldReference() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;

private:
  
  ModelField* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* ModelFieldReference_h */
