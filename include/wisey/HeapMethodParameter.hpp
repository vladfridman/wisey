//
//  HeapMethodParameter.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapMethodParameter_h
#define HeapMethodParameter_h

namespace wisey {
  
/**
 * Represents a heap variable that is a method parameter.
 *
 * This is the same as LocalHeapVariable only returns true on existsInOuterScope()
 */
class HeapMethodParameter : public IVariable {
  
  std::string mName;
  const IType* mType;
  llvm::Value* mValue;
  
public:
  
  HeapMethodParameter(std::string name, const IType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~HeapMethodParameter() {
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
  
};
  
} /* namespace wisey */

#endif /* HeapMethodParameter_h */
