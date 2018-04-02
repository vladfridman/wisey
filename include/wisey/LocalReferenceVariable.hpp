//
//  LocalReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalReferenceVariable_h
#define LocalReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a variable that is a reference to an object
   */
  class LocalReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const IReferenceType* mType;
    llvm::Value* mValueStore;
    bool mIsInitialized;
    
  public:
    
    LocalReferenceVariable(std::string name, const IReferenceType* type, llvm::Value* valueStore);
    
    ~LocalReferenceVariable();
    
    std::string getName() const override;
    
    const IReferenceType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
    void setToNull(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* LocalReferenceVariable_h */

