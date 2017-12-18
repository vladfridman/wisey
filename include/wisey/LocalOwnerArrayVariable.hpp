//
//  LocalOwnerArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalOwnerArrayVariable_h
#define LocalOwnerArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of array type where each element is an owner reference
   */
  class LocalOwnerArrayVariable : public IOwnerVariable {
    
    std::string mName;
    const ArrayType* mType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalOwnerArrayVariable(std::string name, const ArrayType* type, llvm::Value* valueStore);
    
    ~LocalOwnerArrayVariable();
    
    std::string getName() const override;
    
    const ArrayType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context) const override;
    
    void setToNull(IRGenerationContext& context) override;

  };
  
} /* namespace wisey */

#endif /* LocalOwnerArrayVariable_h */
