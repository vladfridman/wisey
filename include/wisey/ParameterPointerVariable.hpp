//
//  ParameterPointerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterPointerVariable_h
#define ParameterPointerVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/ILLVMPointerType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function parameter that is a native type
   */
  class ParameterPointerVariable : public IReferenceVariable {
    
    std::string mName;
    const ILLVMPointerType* mType;
    llvm::Value* mValue;
    
  public:
    
    ParameterPointerVariable(std::string name, const ILLVMPointerType* type, llvm::Value* value);
    
    ~ParameterPointerVariable();
    
    std::string getName() const override;
    
    const ILLVMPointerType* getType() const override;
    
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

#endif /* ParameterPointerVariable_h */
