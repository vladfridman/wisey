//
//  IntType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IntType_h
#define IntType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents integer expression type
 */
class IntType : public IPrimitiveType {
  
public:
  
  IntType() { }
  
  ~IntType() { }
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;

  std::string getFormat() const override;
  
};
  
} /* namespace wisey */

#endif /* IntType_h */
