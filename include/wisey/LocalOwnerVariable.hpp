//
//  LocalOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalOwnerVariable_h
#define LocalOwnerVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IOwnerType.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a variable that is an object owner
   */
  class LocalOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const IOwnerType* mType;
    llvm::Value* mValueStore;
    bool mIsInitialized;
    
  public:
    
    LocalOwnerVariable(std::string name, const IOwnerType* type, llvm::Value* valueStore);
    
    ~LocalOwnerVariable();
    
    std::string getName() const override;
    
    const IOwnerType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context) const override;
    
    void setToNull(IRGenerationContext& context) override;
    
  };
  
} /* namespace wisey */

#endif /* LocalOwnerVariable_h */

