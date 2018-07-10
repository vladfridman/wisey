//
//  FieldArrayReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldArrayReferenceVariable_h
#define FieldArrayReferenceVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of array reference type
   */
  class FieldArrayReferenceVariable : public IFieldVariable, public IReferenceVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;

  public:
    
    FieldArrayReferenceVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldArrayReferenceVariable();
    
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
    
  private:
    
    llvm::Value* generateWholeArrayAssignment(IRGenerationContext& context,
                                              IExpression* assignToExpression,
                                              int line);
    
    llvm::Value* generateArrayElementAssignment(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                std::vector<const IExpression*> arrayIndices,
                                                int line);
    
  };
  
} /* namespace wisey */

#endif /* FieldArrayReferenceVariable_h */

