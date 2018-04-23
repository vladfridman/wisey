//
//  FieldImmutableArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldImmutableArrayOwnerVariable_h
#define FieldImmutableArrayOwnerVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IOwnerVariable.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of immutable array owner type
   */
  class FieldImmutableArrayOwnerVariable : public IFieldVariable, public IOwnerVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldImmutableArrayOwnerVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldImmutableArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ImmutableArrayOwnerType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context, int line) const override;
    
    void setToNull(IRGenerationContext& context) override;
    
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

#endif /* FieldImmutableArrayOwnerVariable_h */