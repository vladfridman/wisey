//
//  LongType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LongType_h
#define LongType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents long integer expression type
 */
class LongType : public IPrimitiveType {
  
  const ArrayElementType* mArrayElementType;

public:
  
  LongType();
  
  ~LongType();
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  std::string getFormat() const override;
  
  const ArrayElementType* getArrayElementType() const override;

};
  
} /* namespace wisey */

#endif /* LongType_h */
