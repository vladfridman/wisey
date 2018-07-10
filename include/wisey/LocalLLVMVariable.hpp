//
//  LocalLLVMVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalLLVMVariable_h
#define LocalLLVMVariable_h

#include "wisey/ILLVMType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of non-pointer llvm type
   */
  class LocalLLVMVariable : public IVariable {
    
    std::string mName;
    const ILLVMType* mType;
    llvm::Value* mValueStore;
    int mLine;
    
  public:
    
    LocalLLVMVariable(std::string name,
                      const ILLVMType* type,
                      llvm::Value* valueStore,
                      int line);
    
    ~LocalLLVMVariable();
    
    std::string getName() const override;
    
    const ILLVMType* getType() const override;
    
    bool isField() const override;
    
    bool isStatic() const override;
    
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

#endif /* LocalLLVMVariable_h */
