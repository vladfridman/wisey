//
//  FieldArrayOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldArrayOwnerVariable_h
#define FieldArrayOwnerVariable_h

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of array owner type
   */
  class FieldArrayOwnerVariable : public IFieldVariable, public IOwnerVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldArrayOwnerVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldArrayOwnerVariable();
    
    std::string getName() const override;
    
    const ArrayOwnerType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context) const override;
    
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

#endif /* FieldArrayOwnerVariable_h */