//
//  FieldImmutableArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldImmutableArrayReferenceVariable_h
#define FieldImmutableArrayReferenceVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IReferenceVariable.hpp"
#include "wisey/ImmutableArrayType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of immutable array reference type
   */
  class FieldImmutableArrayReferenceVariable : public IFieldVariable, public IReferenceVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;
    
  public:
    
    FieldImmutableArrayReferenceVariable(std::string name,
                                         const IConcreteObjectType* object,
                                         int line);
    
    ~FieldImmutableArrayReferenceVariable();
    
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
    
    void decrementReferenceCounter(IRGenerationContext& context, int line) const override;
    
  private:
    
    llvm::Value* generateWholeArrayAssignment(IRGenerationContext& context,
                                              IExpression* assignToExpression,
                                              int line);
    
    llvm::Value* generateArrayElementAssignment(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                std::vector<const IExpression*> arrayIndices,
                                                int line);
    
  };
  
} /* namespace wisey */

#endif /* FieldImmutableArrayReferenceVariable_h */
