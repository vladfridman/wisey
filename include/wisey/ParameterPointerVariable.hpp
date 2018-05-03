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
#include "wisey/IReferenceVariable.hpp"
#include "wisey/LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function parameter that is a native type
   */
  class ParameterPointerVariable : public IVariable {
    
    std::string mName;
    const LLVMPointerType* mType;
    llvm::Value* mValue;
    int mLine;

  public:
    
    ParameterPointerVariable(std::string name,
                             const LLVMPointerType* type,
                             llvm::Value* value,
                             int line);
    
    ~ParameterPointerVariable();
    
    std::string getName() const override;
    
    const LLVMPointerType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    int getLine() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterPointerVariable_h */
