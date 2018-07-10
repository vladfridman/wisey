//
//  FieldPrimitiveVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldPrimitiveVariable_h
#define FieldPrimitiveVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of primitive type
   */
  class FieldPrimitiveVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;
    
  public:
    
    FieldPrimitiveVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldPrimitiveVariable();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
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
    
  };
  
} /* namespace wisey */

#endif /* FieldPrimitiveVariable_h */

