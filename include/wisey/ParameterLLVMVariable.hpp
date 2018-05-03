//
//  ParameterLLVMVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterLLVMVariable_h
#define ParameterLLVMVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/ILLVMType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a non pointer llvm type method parameter
   */
  class ParameterLLVMVariable : public IVariable {
    
    std::string mName;
    const ILLVMType* mType;
    llvm::Value* mValue;
    
  public:
    
    ParameterLLVMVariable(std::string name, const ILLVMType* type, llvm::Value* value);
    
    ~ParameterLLVMVariable();
    
    std::string getName() const override;
    
    const ILLVMType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterLLVMVariable_h */
