//
//  FieldPointerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldPointerVariable_h
#define FieldPointerVariable_h

#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IObjectType.hpp"
#include "IVariable.hpp"
#include "LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of llvm pointer type
   */
  class FieldPointerVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;
    
  public:
    
    FieldPointerVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldPointerVariable();
    
    std::string getName() const override;
    
    const LLVMPointerType* getType() const override;
    
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

#endif /* FieldPointerVariable_h */
