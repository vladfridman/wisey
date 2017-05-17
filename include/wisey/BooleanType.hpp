//
//  BooleanType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanType_h
#define BooleanType_h

#include "wisey/IType.hpp"

namespace yazyk {
  
/**
 * Represents char expression type
 */
class BooleanType : public IType {
    
public:
  
  BooleanType() { }
  
  ~BooleanType() { }
  
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

#endif /* BooleanType_h */
