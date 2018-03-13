//
//  ParameterPrimitiveVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterPrimitiveVariable_h
#define ParameterPrimitiveVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IPrimitiveType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a primitive type method parameter
 */
class ParameterPrimitiveVariable : public IVariable {
    
  std::string mName;
  const IPrimitiveType* mType;
  llvm::Value* mValue;
  
public:
  
  ParameterPrimitiveVariable(std::string name, const IPrimitiveType* type, llvm::Value* value);
  
  ~ParameterPrimitiveVariable();
  
  std::string getName() const override;
  
  const IPrimitiveType* getType() const override;
  
  bool isField() const override;

  bool isSystem() const override;

  llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression,
                                    std::vector<const IExpression*> arrayIndices,
                                    int line) override;
  
};
  
} /* namespace wisey */

#endif /* ParameterPrimitiveVariable_h */

