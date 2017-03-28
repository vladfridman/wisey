//
//  ObjectFieldVariable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectFieldVariable_h
#define ObjectFieldVariable_h

#include "yazyk/IObjectWithFieldsType.hpp"
#include "yazyk/IVariable.hpp"
#include "yazyk/Model.hpp"

namespace yazyk {

/**
 * Represents a local variable allocated on the stack
 */
class ObjectFieldVariable : public IVariable {
  
  std::string mName;
  llvm::Value* mValue;
  IObjectWithFieldsType* mObject;
  
public:
  
  ObjectFieldVariable(std::string name, llvm::Value* value, IObjectWithFieldsType* object)
    : mName(name), mValue(value), mObject(object) { }
  
  ~ObjectFieldVariable() {}
  
  std::string getName() const override;
  
  IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression& assignToExpression) override;
  
  void free(IRGenerationContext& context) const override;
  
private:
  
  ModelField* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* ObjectFieldVariable_h */
