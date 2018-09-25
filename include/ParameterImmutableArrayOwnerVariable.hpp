//
//  ParameterImmutableArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterImmutableArrayOwnerVariable_h
#define ParameterImmutableArrayOwnerVariable_h

#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IOwnerVariable.hpp"
#include "ImmutableArrayOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is of immutable array owner type
   */
  class ParameterImmutableArrayOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    ParameterImmutableArrayOwnerVariable(std::string name,
                                         const ImmutableArrayOwnerType* immutableArrayOwnerType,
                                         llvm::Value* valueStore,
                                         int line);
    
    ~ParameterImmutableArrayOwnerVariable();
    
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
    
  };
  
} /* namespace wisey */

#endif /* ParameterImmutableArrayOwnerVariable_h */
