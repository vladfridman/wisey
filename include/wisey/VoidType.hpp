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
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;
  
  bool isReference() const override;

  std::string getFormat() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void allocateLocalVariable(IRGenerationContext& context, std::string name) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* VoidType_h */
