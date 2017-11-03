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
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a primitive type method parameter
 */
class ParameterPrimitiveVariable : public IVariable {
    
  std::string mName;
  const IType* mType;
  llvm::Value* mValue;
  
public:
  
  ParameterPrimitiveVariable(std::string name, const IType* type, llvm::Value* value);
  
  ~ParameterPrimitiveVariable();
  
  std::string getName() const override;
  
  const IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;
  
  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;
  
  void setToNull(IRGenerationContext& context) override;
  
};
  
} /* namespace wisey */

#endif /* ParameterPrimitiveVariable_h */

