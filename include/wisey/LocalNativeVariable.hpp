//
//  LocalNativeVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalNativeVariable_h
#define LocalNativeVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of native type
   */
  class LocalNativeVariable : public IVariable {
    
    std::string mName;
    const IType* mType;
    llvm::Value* mValueStore;
    
  public:
    
    LocalNativeVariable(std::string name, const IType* type, llvm::Value* valueStore);
    
    ~LocalNativeVariable();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;

    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* LocalNativeVariable_h */
