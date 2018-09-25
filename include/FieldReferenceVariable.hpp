//
//  FieldReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldReferenceVariable_h
#define FieldReferenceVariable_h

#include "IConcreteObjectType.hpp"
#include "IFieldVariable.hpp"
#include "IObjectType.hpp"
#include "IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of reference type
   */
  class FieldReferenceVariable : public IFieldVariable, public IReferenceVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;

  public:
    
    FieldReferenceVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldReferenceVariable();
    
    std::string getName() const override;
    
    const IReferenceType* getType() const override;
    
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

#endif /* FieldReferenceVariable_h */

