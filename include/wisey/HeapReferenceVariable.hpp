//
//  HeapReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapReferenceVariable_h
#define HeapReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents a variable that is a reference to a heap object
 */
class HeapReferenceVariable : public IVariable {
  
  std::string mName;
  const IType* mType;
  llvm::Value* mValue;
  
public:
  
  HeapReferenceVariable(std::string name, const IType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~HeapReferenceVariable() {
  }
  
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

#endif /* HeapReferenceVariable_h */
