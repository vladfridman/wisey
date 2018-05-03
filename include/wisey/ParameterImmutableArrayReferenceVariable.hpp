//
//  ParameterImmutableArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterImmutableArrayReferenceVariable_h
#define ParameterImmutableArrayReferenceVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IReferenceVariable.hpp"
#include "wisey/ImmutableArrayType.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is of immutable array reference type
   */
  class ParameterImmutableArrayReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const ImmutableArrayType* mImmutableArrayType;
    llvm::Value* mValue;
    int mLine;

  public:
    
    ParameterImmutableArrayReferenceVariable(std::string name,
                                             const ImmutableArrayType* immutableArrayType,
                                             llvm::Value* value,
                                             int line);

    ~ParameterImmutableArrayReferenceVariable();
    
    std::string getName() const override;
    
    const ImmutableArrayType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
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

#endif /* ParameterImmutableArrayReferenceVariable_h */
