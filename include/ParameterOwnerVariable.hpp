//
//  ParameterOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterOwnerVariable_h
#define ParameterOwnerVariable_h

#include "IObjectOwnerType.hpp"
#include "IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is an object owner.
   */
  class ParameterOwnerVariable : public IOwnerVariable {
    
    std::string mName;
    const IOwnerType* mType;
    llvm::Value* mValueStore;
    int mLine;
    
  public:
    
    ParameterOwnerVariable(std::string name,
                           const IOwnerType* type,
                           llvm::Value* valueStore,
                           int line);
    
    ~ParameterOwnerVariable();
    
    std::string getName() const override;
    
    const IOwnerType* getType() const override;
    
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


#endif /* ParameterOwnerVariable_h */

