//
//  HeapReferenceMethodParameter.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapReferenceMethodParameter_h
#define HeapReferenceMethodParameter_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a method parameter that is a reference to a heap object.
 *
 * This is the same as HeapReferenceVariable only returns true on existsInOuterScope()
 */
class HeapReferenceMethodParameter : public IVariable {
  
  std::string mName;
  const IObjectType* mType;
  llvm::Value* mValue;
  
public:
  
  HeapReferenceMethodParameter(std::string name, const IObjectType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~HeapReferenceMethodParameter() {
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

#endif /* HeapReferenceMethodParameter_h */
