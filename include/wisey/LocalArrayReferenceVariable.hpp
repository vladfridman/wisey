//
//  LocalArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalArrayReferenceVariable_h
#define LocalArrayReferenceVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array owner type where each element could be of any type
   */
  class LocalArrayReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const ArrayType* mType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalArrayReferenceVariable(std::string name, const ArrayType* type, llvm::Value* valueStore);
    
    ~LocalArrayReferenceVariable();
    
    std::string getName() const override;
    
    const ArrayType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
  private:
    
    llvm::Value* generateWholeArrayAssignment(IRGenerationContext& context,
                                              IExpression* assignToExpression,
                                              int line);
        
  };
  
} /* namespace wisey */

#endif /* LocalArrayReferenceVariable_h */