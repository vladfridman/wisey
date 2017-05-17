//
//  VoidType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef VoidType_h
#define VoidType_h

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include "wisey/IType.hpp"

namespace yazyk {
  
/**
 * Represents void expression type
 */
class VoidType : public IType {
    
public:
  
  VoidType() { }
  
  ~VoidType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
};
  
} /* namespace yazyk */

#endif /* VoidType_h */
