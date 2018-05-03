//
//  FieldPointerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldPointerVariable_h
#define FieldPointerVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of llvm pointer type
   */
  class FieldPointerVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldPointerVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldPointerVariable();
    
    std::string getName() const override;
    
    const LLVMPointerType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
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
