//
//  ParameterPrimitiveVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterPrimitiveVariable_h
#define ParameterPrimitiveVariable_h

#include "IExpression.hpp"
#include "IPrimitiveType.hpp"
#include "IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a primitive type method parameter
   */
  class ParameterPrimitiveVariable : public IVariable {
    
    std::string mName;
    const IPrimitiveType* mType;
    llvm::Value* mValue;
    int mLine;

  public:
    
    ParameterPrimitiveVariable(std::string name,
                               const IPrimitiveType* type,
                               llvm::Value* value,
                               int line);
    
    ~ParameterPrimitiveVariable();
    
    std::string getName() const override;
    
    const IPrimitiveType* getType() const override;
    
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

#endif /* ParameterPrimitiveVariable_h */

