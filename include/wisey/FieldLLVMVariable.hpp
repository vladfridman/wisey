//
//  FieldLLVMVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldLLVMVariable_h
#define FieldLLVMVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/ILLVMType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of native type
   */
  class FieldLLVMVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;
    
  public:
    
    FieldLLVMVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldLLVMVariable();
    
    std::string getName() const override;
    
    const ILLVMType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
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

#endif /* FieldLLVMVariable_h */

