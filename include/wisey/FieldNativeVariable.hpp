//
//  FieldNativeVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FieldNativeVariable_h
#define FieldNativeVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IReferenceVariable.hpp"
#include "wisey/NativeType.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of native type
   */
  class FieldNativeVariable : public IFieldVariable, public IVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    
  public:
    
    FieldNativeVariable(std::string name, const IConcreteObjectType* object);
    
    ~FieldNativeVariable();
    
    std::string getName() const override;
    
    const NativeType* getType() const override;
    
    bool isField() const override;

    bool isSystem() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* FieldNativeVariable_h */
