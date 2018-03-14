//
//  ParameterArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterArrayOwnerVariable_h
#define ParameterArrayOwnerVariable_h

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is of array owner type
   */
  class ParameterArrayOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const ArrayOwnerType* mArrayOwnerType;
    llvm::Value* mValueStore;

  public:
    
    ParameterArrayOwnerVariable(std::string name,
                                const ArrayOwnerType* arrayOwnerType,
                                llvm::Value* valueStore);
    
    ~ParameterArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ArrayOwnerType* getType() const override;
    
    bool isField() const override;

    bool isSystem() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;

    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context) const override;
    
    void setToNull(IRGenerationContext& context) override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterArrayOwnerVariable_h */
