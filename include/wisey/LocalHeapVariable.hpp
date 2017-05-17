//
//  LocalHeapVariable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalHeapVariable_h
#define LocalHeapVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents a local variable allocated on the heap
 */
class LocalHeapVariable : public IVariable {
  
  std::string mName;
  IType* mType;
  llvm::Value* mValue;
  
public:
  
  LocalHeapVariable(std::string name, IType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~LocalHeapVariable() {
  }
  
  std::string getName() const override;
  
  IType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression& assignToExpression) override;

  void free(IRGenerationContext& context) const override;
};

} /* namespace wisey */

#endif /* LocalHeapVariable_h */
