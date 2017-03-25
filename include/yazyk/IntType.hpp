//
//  IntType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IntType_h
#define IntType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
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
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
};
  
} /* namespace yazyk */

#endif /* IntType_h */
