//
//  LocalPointerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalPointerVariable_h
#define LocalPointerVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of native type
   */
  class LocalPointerVariable : public IVariable {
    
    std::string mName;
    const LLVMPointerType* mType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    LocalPointerVariable(std::string name,
                         const LLVMPointerType* type,
                         llvm::Value* valueStore,
                         int line);
    
    ~LocalPointerVariable();
    
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

#endif /* LocalPointerVariable_h */

