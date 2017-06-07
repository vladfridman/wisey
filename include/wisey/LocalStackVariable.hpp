//
//  LocalStackVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalStackVariable_h
#define LocalStackVariable_h

#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents a local variable allocated on the stack
 */
class LocalStackVariable : public IVariable {

  std::string mName;
  const IType* mType;
  llvm::Value* mValue;
  
public:

  LocalStackVariable(std::string name, const IType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~LocalStackVariable() {}
  
  std::string getName() const override;
  
  const IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;

  void free(IRGenerationContext& context) const override;
};

} /* namespace wisey */

#endif /* LocalStackVariable_h */
