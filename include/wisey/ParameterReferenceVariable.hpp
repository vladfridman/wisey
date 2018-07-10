//
//  ParameterReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterReferenceVariable_h
#define ParameterReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is a reference to an object.
   */
  class ParameterReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const IReferenceType* mType;
    llvm::Value* mValue;
    int mLine;

  public:
    
    ParameterReferenceVariable(std::string name,
                               const IReferenceType* type,
                               llvm::Value* value,
                               int line);
    
    ~ParameterReferenceVariable();
    
    std::string getName() const override;
    
    const IReferenceType* getType() const override;
    
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
    
  };
  
} /* namespace wisey */

#endif /* ParameterReferenceVariable_h */

