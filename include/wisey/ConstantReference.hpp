//
//  ConstantReference.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ConstantReference_h
#define ConstantReference_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a constant reference that looks like my.packge.MMyObject.MYCONSTANT
 */
class ConstantReference : public IExpression {
  
  const IObjectTypeSpecifier* mObjectTypeSpecifier;
  std::string mConstantName;
  
public:
  ConstantReference(const IObjectTypeSpecifier* objectTypeSpecifier,
                    std::string constantName);
  
  ~ConstantReference();
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
private:
  
  const IObjectType* getObjectType(IRGenerationContext& context) const;
};
  
} /* namespace wisey */

#endif /* ConstantReference_h */
