//
//  CharType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CharType_h
#define CharType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {

/**
 * Represents char expression type
 */
class CharType : public IPrimitiveType {

  const ArrayElementType* mArrayElementType;

public:
  
  CharType();
  
  ~CharType();
  
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

#endif /* CharType_h */
