//
//  LocalArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalArrayReferenceVariable_h
#define LocalArrayReferenceVariable_h

#include "ArrayType.hpp"
#include "IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array type where each element could be of any type
   */
  class LocalArrayReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const ArrayType* mArrayType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    LocalArrayReferenceVariable(std::string name,
                                const ArrayType* arrayType,
                                llvm::Value* valueStore,
                                int line);
    
    ~LocalArrayReferenceVariable();
    
    std::string getName() const override;
    
    const ArrayType* getType() const override;
    
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

#endif /* LocalArrayReferenceVariable_h */

