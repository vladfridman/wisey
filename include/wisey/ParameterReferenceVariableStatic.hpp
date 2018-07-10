//
//  ParameterReferenceVariableStatic.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterReferenceVariableStatic_h
#define ParameterReferenceVariableStatic_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is a reference that does not require ref count adjustment
   */
  class ParameterReferenceVariableStatic : public IReferenceVariable {
    
    std::string mName;
    const IObjectType* mType;
    llvm::Value* mValue;
    int mLine;
    
  public:
    
    ParameterReferenceVariableStatic(std::string name,
                                     const IObjectType* type,
                                     llvm::Value* value,
                                     int line);
    
    ~ParameterReferenceVariableStatic();
    
    std::string getName() const override;
    
    const IObjectType* getType() const override;
    
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

#endif /* ParameterReferenceVariableStatic_h */

