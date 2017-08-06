//
//  HeapReferenceMethodParameter.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapReferenceMethodParameter_h
#define HeapReferenceMethodParameter_h

namespace wisey {
  
/**
 * Represents a method parameter that is a reference to a heap object.
 *
 * This is the same as HeapReferenceVariable only returns true on existsInOuterScope()
 */
class HeapReferenceMethodParameter : public IVariable {
  
  std::string mName;
  const IType* mType;
  llvm::Value* mValue;
  
public:
  
  HeapReferenceMethodParameter(std::string name, const IType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~HeapReferenceMethodParameter() {
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

#endif /* HeapReferenceMethodParameter_h */
