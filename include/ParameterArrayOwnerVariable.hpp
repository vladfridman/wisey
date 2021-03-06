//
//  ParameterArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterArrayOwnerVariable_h
#define ParameterArrayOwnerVariable_h

#include "ArrayOwnerType.hpp"
#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is of array owner type
   */
  class ParameterArrayOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const ArrayOwnerType* mArrayOwnerType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    ParameterArrayOwnerVariable(std::string name,
                                const ArrayOwnerType* arrayOwnerType,
                                llvm::Value* valueStore,
                                int line);
    
    ~ParameterArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ArrayOwnerType* getType() const override;
    
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

#endif /* ParameterArrayOwnerVariable_h */

