//
//  FieldOwnerArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldOwnerArrayVariable_h
#define FieldOwnerArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of owner array type
   */
  class FieldOwnerArrayVariable : public IFieldVariable, public IOwnerVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldOwnerArrayVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldOwnerArrayVariable();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context) const override;
    
    void setToNull(IRGenerationContext& context) override;

  };
  
} /* namespace wisey */

#endif /* FieldOwnerArrayVariable_h */
