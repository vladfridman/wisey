//
//  ControllerOwner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerOwner_h
#define ControllerOwner_h

#include "wisey/Controller.hpp"
#include "wisey/IObjectOwnerType.hpp"

namespace wisey {
  
/**
 * Owner type for controller
 */
class ControllerOwner : public IObjectOwnerType {
  
  Controller* mController;
  
public:
  
  ControllerOwner(Controller* controller) : mController(controller) { }
  
  ~ControllerOwner() { }
  
  Controller* getObject() const override;

  std::string getName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;

};
  
} /* namespace wisey */

#endif /* ControllerOwner_h */
