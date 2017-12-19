//
//  LocalReferenceArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalReferenceArrayVariable_h
#define LocalReferenceArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array type where each element is a reference
   */
  class LocalReferenceArrayVariable : public IReferenceVariable {
    
    std::string mName;
    const ArrayType* mType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalReferenceArrayVariable(std::string name, const ArrayType* type, llvm::Value* valueStore);
    
    ~LocalReferenceArrayVariable();
    
    std::string getName() const override;
    
    const ArrayType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* LocalReferenceArrayVariable_h */
