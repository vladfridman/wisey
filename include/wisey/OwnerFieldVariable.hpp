//
//  OwnerFieldVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef OwnerFieldVariable_h
#define OwnerFieldVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"

namespace wisey {
  
/**
 * Represents an object field that is of owner type
 */
class OwnerFieldVariable : public IFieldVariable {
    
  std::string mName;
  llvm::Value* mValue;
  const IConcreteObjectType* mObject;
  
public:
  
  OwnerFieldVariable(std::string name, llvm::Value* value, const IConcreteObjectType* object)
  : mName(name), mValue(value), mObject(object) { }
  
  ~OwnerFieldVariable() {}
  
  std::string getName() const override;
  
  const IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;
  
  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;
  
  void setToNull(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* OwnerFieldVariable_h */