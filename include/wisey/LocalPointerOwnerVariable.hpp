//
//  LocalPointerOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalPointerOwnerVariable_h
#define LocalPointerOwnerVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of native pointer type that is owner of its object
   */
  class LocalPointerOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const LLVMPointerOwnerType* mType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalPointerOwnerVariable(std::string name,
                              const LLVMPointerOwnerType* type,
                              llvm::Value* valueStore);
    
    ~LocalPointerOwnerVariable();
    
    std::string getName() const override;
    
    const LLVMPointerOwnerType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void setToNull(IRGenerationContext& context) override;
    
    void free(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* LocalPointerOwnerVariable_h */
