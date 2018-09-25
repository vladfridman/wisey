//
//  FieldImmutableArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldImmutableArrayOwnerVariable_h
#define FieldImmutableArrayOwnerVariable_h

#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IOwnerVariable.hpp"
#include "ImmutableArrayOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of immutable array owner type
   */
  class FieldImmutableArrayOwnerVariable : public IFieldVariable, public IOwnerVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;

  public:
    
    FieldImmutableArrayOwnerVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldImmutableArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ImmutableArrayOwnerType* getType() const override;
    
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
    
    void free(IRGenerationContext& context, llvm::Value* exception, int line) const override;
    
    void setToNull(IRGenerationContext& context, int line) override;
    
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
