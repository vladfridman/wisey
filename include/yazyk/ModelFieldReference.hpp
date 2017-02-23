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
#include "yazyk/Model.hpp"

namespace yazyk {

/**
 * Represents an identifier that refers to model's field
 */
class ModelFieldReference : public IExpression {
  
  IExpression& mExpression;
  std::string mFieldName;
  
public:
  
  ModelFieldReference(IExpression& expression, std::string fieldName)
    : mExpression(expression), mFieldName(fieldName) { }
  
  ~ModelFieldReference() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;

private:
  
   ModelField* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* ModelFieldReference_h */
