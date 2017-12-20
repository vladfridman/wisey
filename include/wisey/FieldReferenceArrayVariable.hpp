//
//  FieldReferenceArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldReferenceArrayVariable_h
#define FieldReferenceArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of reference array type
   */
  class FieldReferenceArrayVariable : public IFieldVariable, public IReferenceVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;

  public:
    
    FieldReferenceArrayVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldReferenceArrayVariable();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* FieldReferenceArrayVariable_h */
