//
//  ParameterOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterOwnerVariable_h
#define ParameterOwnerVariable_h

#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a method parameter that is an object owner.
 *
 * This is the same as LocalOwnerVariable only returns true on existsInOuterScope()
 */
class ParameterOwnerVariable : public IVariable {
    
  std::string mName;
  const IObjectOwnerType* mType;
  llvm::Value* mValueStore;
  
public:
  
  ParameterOwnerVariable(std::string name, const IObjectOwnerType* type, llvm::Value* valueStore);
  
  ~ParameterOwnerVariable();
  
  std::string getName() const override;
  
  const IType* getType() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;
  
  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;
  
  void setToNull(IRGenerationContext& context) override;
  
};
  
} /* namespace wisey */


#endif /* ParameterOwnerVariable_h */
