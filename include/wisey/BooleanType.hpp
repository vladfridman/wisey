//
//  BooleanType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanType_h
#define BooleanType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents char expression type
 */
class BooleanType : public IPrimitiveType {
    
public:
  
  BooleanType() { }
  
  ~BooleanType() { }
  
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

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* BooleanType_h */
