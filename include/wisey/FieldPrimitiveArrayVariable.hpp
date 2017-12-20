//
//  FieldPrimitiveArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldPrimitiveArrayVariable_h
#define FieldPrimitiveArrayVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of primitive array type
   */
  class FieldPrimitiveArrayVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldPrimitiveArrayVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldPrimitiveArrayVariable();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* FieldPrimitiveArrayVariable_h */
