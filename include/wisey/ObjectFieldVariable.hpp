//
//  ObjectFieldVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectFieldVariable_h
#define ObjectFieldVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/Model.hpp"

namespace wisey {

/**
 * Represents a local variable allocated on the stack
 */
class ObjectFieldVariable : public IVariable {
  
  std::string mName;
  llvm::Value* mValue;
  const IConcreteObjectType* mObject;
  
public:
  
  ObjectFieldVariable(std::string name, llvm::Value* value, const IConcreteObjectType* object)
    : mName(name), mValue(value), mObject(object) { }
  
  ~ObjectFieldVariable() {}
  
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

private:
  
  Field* checkAndFindField(IRGenerationContext& context) const;
  
  llvm::GetElementPtrInst* getFieldPointer(IRGenerationContext& context) const;
  
};

} /* namespace wisey */

#endif /* ObjectFieldVariable_h */
