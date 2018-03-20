//
//  ParameterNativeVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterNativeVariable_h
#define ParameterNativeVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/ILLVMType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function parameter that is a native type
   */
  class ParameterNativeVariable : public IReferenceVariable {
    
    std::string mName;
    const ILLVMType* mType;
    llvm::Value* mValue;
    
  public:
    
    ParameterNativeVariable(std::string name, const ILLVMType* type, llvm::Value* value);
    
    ~ParameterNativeVariable();
    
    std::string getName() const override;
    
    const ILLVMType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterNativeVariable_h */
