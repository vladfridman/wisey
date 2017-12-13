//
//  ArrayType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayType_h
#define ArrayType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents the array type
 */
class ArrayType : public IType {
  
  const IType* mBaseType;
  unsigned long mSize;
  
public:
  
  ArrayType(const IType* baseType, unsigned long size);
  
  ~ArrayType();
  
  /**
   * Return array base type
   */
  const IType* getBaseType() const;
  
  /**
   * Return array size
   */
  unsigned long getSize() const;
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmContext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
};
  
}

#endif /* ArrayType_h */
