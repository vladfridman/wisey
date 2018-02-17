//
//  UndefinedType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef UndefinedType_h
#define UndefinedType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents the type that could not be determined
 */
class UndefinedType : public IType {
    
public:
  
  UndefinedType() { }
  
  ~UndefinedType() { }
  
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

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void allocateVariable(IRGenerationContext& context, std::string name) const override;

  static UndefinedType* UNDEFINED_TYPE;
  
};

} /* namespace wisey */

#endif /* UndefinedType_h */
