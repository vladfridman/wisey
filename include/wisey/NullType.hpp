//
//  NullType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NullType_h
#define NullType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents a type used for null token
 */
class NullType : public IType {
    
public:
  
  NullType() { }
  
  ~NullType() { }
  
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

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void allocateVariable(IRGenerationContext& context, std::string name) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;
  
  static NullType* NULL_TYPE;

};
  
} /* namespace wisey */

#endif /* NullType_h */
