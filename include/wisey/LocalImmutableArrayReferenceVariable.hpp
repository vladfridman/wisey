//
//  LocalImmutableArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalImmutableArrayReferenceVariable_h
#define LocalImmutableArrayReferenceVariable_h

#include "wisey/IReferenceVariable.hpp"
#include "wisey/ImmutableArrayType.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array type where each element could be of any type
   */
  class LocalImmutableArrayReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const ImmutableArrayType* mImmutableArrayType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalImmutableArrayReferenceVariable(std::string name,
                                         const ImmutableArrayType* immutableArrayType,
                                         llvm::Value* valueStore);
    
    ~LocalImmutableArrayReferenceVariable();
    
    std::string getName() const override;
    
    const ImmutableArrayType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
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

#endif /* LocalImmutableArrayReferenceVariable_h */
