//
//  ReferenceFieldVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ReferenceFieldVariable_h
#define ReferenceFieldVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"

namespace wisey {

/**
 * Represents an object field that is of reference type
 */
class ReferenceFieldVariable : public IFieldVariable {
  
  std::string mName;
  llvm::Value* mValue;
  const IConcreteObjectType* mObject;
  
public:
  
  ReferenceFieldVariable(std::string name,
                               llvm::Value* value,
                               const IConcreteObjectType* object) :
  mName(name),
  mValue(value),
  mObject(object) { }
  
  ~ReferenceFieldVariable() {}
  
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

#endif /* ReferenceFieldVariable_h */
