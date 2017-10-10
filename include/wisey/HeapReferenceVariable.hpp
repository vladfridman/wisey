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
#include "wisey/IObjectType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents a variable that is a reference to a heap object
 */
class HeapReferenceVariable : public IVariable {
  
  std::string mName;
  const IObjectType* mType;
  llvm::Value* mValue;
  bool mIsInitialized;
  
public:
  
  HeapReferenceVariable(std::string name, const IObjectType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value), mIsInitialized(false) { }
  
  ~HeapReferenceVariable() {
  }
  
  std::string getName() const override;
  
  const IObjectType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;

  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;
  
  void setToNull(IRGenerationContext& context) override;

};

} /* namespace wisey */

#endif /* HeapReferenceVariable_h */
