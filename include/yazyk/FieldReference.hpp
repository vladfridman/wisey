//
//  FieldReference.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldReference_h
#define FieldReference_h

#include "yazyk/IExpression.hpp"
#include "yazyk/Model.hpp"

namespace yazyk {

/**
 * Represents an identifier that refers to model's field
 */
class FieldReference : public IExpression {
  
  IExpression& mExpression;
  std::string mFieldName;
  
public:
  
  FieldReference(IExpression& expression, std::string fieldName)
    : mExpression(expression), mFieldName(fieldName) { }
  
  ~FieldReference() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

private:
  
   Field* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* FieldReference_h */
