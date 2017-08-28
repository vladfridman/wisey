//
//  HeapOwnerMethodParameter.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapOwnerMethodParameter_h
#define HeapOwnerMethodParameter_h

namespace wisey {
  
/**
 * Represents a method parameter that is an owner of a heap object.
 *
 * This is the same as HeapOwnerVariable only returns true on existsInOuterScope()
 */
class HeapOwnerMethodParameter : public IVariable {
    
  std::string mName;
  const IObjectOwnerType* mType;
  llvm::Value* mValue;
  
public:
  
  HeapOwnerMethodParameter(std::string name, const IObjectOwnerType* type, llvm::Value* value)
  : mName(name), mType(type), mValue(value) { }
  
  ~HeapOwnerMethodParameter() {
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


#endif /* HeapOwnerMethodParameter_h */
