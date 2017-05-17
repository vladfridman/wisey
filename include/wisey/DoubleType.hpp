//
//  DoubleType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DoubleType_h
#define DoubleType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents double expression type
 */
class DoubleType : public IType {
    
public:
  
  DoubleType() { }
  
  ~DoubleType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
};
  
} /* namespace wisey */

#endif /* DoubleType_h */
