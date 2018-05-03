//
//  LocalArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalArrayOwnerVariable_h
#define LocalArrayOwnerVariable_h

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array owner type where each element could be of any type
   */
  class LocalArrayOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const ArrayOwnerType* mArrayOwnerType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    LocalArrayOwnerVariable(std::string name,
                            const ArrayOwnerType* arrayOwnerType,
                            llvm::Value* valueStore,
                            int line);
    
    ~LocalArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ArrayOwnerType* getType() const override;
    
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
    
    void setToNull(IRGenerationContext& context, int line) override;
    
    void free(IRGenerationContext& context, int line) const override;
    
  private:
    
    llvm::Value* generateWholeArrayAssignment(IRGenerationContext& context,
                                              IExpression* assignToExpression,
                                              int line);
    
    llvm::Value* generateArrayElementAssignment(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                std::vector<const IExpression*> arrayIndices,
                                                int line);
    
  };
  
} /* namespace wisey */

#endif /* LocalArrayOwnerVariable_h */

