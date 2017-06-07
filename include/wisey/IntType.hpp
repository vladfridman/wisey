//
//  IntType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IntType_h
#define IntType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents integer expression type
 */
class IntType : public IType {
  
public:
  
  IntType() { }
  
  ~IntType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;
};
  
} /* namespace wisey */

#endif /* IntType_h */
