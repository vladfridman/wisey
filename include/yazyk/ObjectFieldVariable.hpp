//
//  ObjectFieldVariable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectFieldVariable_h
#define ObjectFieldVariable_h

#include "yazyk/IVariable.hpp"
#include "yazyk/Model.hpp"

namespace yazyk {

/**
 * Represents a local variable allocated on the stack
 */
class ObjectFieldVariable : public IVariable {
  
  std::string mName;
  llvm::Value* mValue;
  Model* mModel;
  
public:
  
  ObjectFieldVariable(std::string name, llvm::Value* value, Model* model)
    : mName(name), mValue(value), mModel(model) { }
  
  ~ObjectFieldVariable() {}
  
  std::string getName() const override;
  
  IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    llvm::Value* assignValue) override;
  
  void free(llvm::BasicBlock* basicBlock) const override;
  
private:
  
  ModelField* checkAndFindField(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* ObjectFieldVariable_h */
