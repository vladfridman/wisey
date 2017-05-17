//
//  FieldReference.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldReference_h
#define FieldReference_h

#include "wisey/IExpression.hpp"
#include "wisey/Model.hpp"

namespace wisey {

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

} /* namespace wisey */

#endif /* FieldReference_h */
