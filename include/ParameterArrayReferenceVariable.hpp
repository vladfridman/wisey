//
//  ParameterArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterArrayReferenceVariable_h
#define ParameterArrayReferenceVariable_h

#include "ArrayType.hpp"
#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a method parameter that is of array reference type
   */
  class ParameterArrayReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const ArrayType* mArrayType;
    llvm::Value* mValue;
    int mLine;

  public:
    
    ParameterArrayReferenceVariable(std::string name,
                                    const ArrayType* arrayType,
                                    llvm::Value* value,
                                    int line);
    
    ~ParameterArrayReferenceVariable();
    
    std::string getName() const override;
    
    const ArrayType* getType() const override;
    
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
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ParameterArrayReferenceVariable_h */

