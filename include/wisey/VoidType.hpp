//
//  VoidType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef VoidType_h
#define VoidType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents void expression type
 */
class VoidType : public IPrimitiveType {
    
public:
  
  VoidType() { }
  
  ~VoidType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;
  
  std::string getFormat() const override;
  
};
  
} /* namespace wisey */

#endif /* VoidType_h */
