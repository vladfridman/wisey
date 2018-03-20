//
//  ObjectFunctionVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectFunctionVariable_h
#define ObjectFunctionVariable_h

#include "wisey/IVariable.hpp"
#include "wisey/LLVMFunctionType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of native type
   */
  class ObjectFunctionVariable : public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObjectType;
    
  public:
    
    ObjectFunctionVariable(std::string name, const IConcreteObjectType* objectType);
    
    ~ObjectFunctionVariable();
    
    std::string getName() const override;
    
    const LLVMFunctionType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  private:
    
    LLVMFunction* findFunction() const;
    
  };
  
} /* namespace wisey */

#endif /* ObjectFunctionVariable_h */
