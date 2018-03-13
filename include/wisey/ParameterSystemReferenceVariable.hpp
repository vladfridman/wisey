//
//  ParameterSystemReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterSystemReferenceVariable_h
#define ParameterSystemReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is a reference to a system object e.g. this, thread
   */
  class ParameterSystemReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const IObjectType* mType;
    llvm::Value* mValue;
    
  public:
    
    ParameterSystemReferenceVariable(std::string name, const IObjectType* type, llvm::Value* value);
    
    ~ParameterSystemReferenceVariable();
    
    std::string getName() const override;
    
    const IObjectType* getType() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterSystemReferenceVariable_h */