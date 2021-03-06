//
//  LocalImmutableArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalImmutableArrayOwnerVariable_h
#define LocalImmutableArrayOwnerVariable_h

#include "ArrayOwnerType.hpp"
#include "IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of immutable array owner type
   */
  class LocalImmutableArrayOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    LocalImmutableArrayOwnerVariable(std::string name,
                                     const ImmutableArrayOwnerType* immutableArrayOwnerType,
                                     llvm::Value* valueStore,
                                     int line);

    ~LocalImmutableArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ImmutableArrayOwnerType* getType() const override;
    
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
    
    void setToNull(IRGenerationContext& context, int line) override;
    
    void free(IRGenerationContext& context, llvm::Value* exception, int line) const override;
    
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

#endif /* LocalImmutableArrayOwnerVariable_h */
