//
//  InterfaceOwner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceOwner_h
#define InterfaceOwner_h

#include "wisey/Interface.hpp"

namespace wisey {

/**
 * Owner type for interface
 */
class InterfaceOwner : public IObjectOwnerType {
  
  Interface* mInterface;
  
public:
  
  InterfaceOwner(Interface* interface);
  
  ~InterfaceOwner();

  Interface* getObject() const override;

  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;

  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;

};
  
} /* namespace wisey */

#endif /* InterfaceOwner_h */
