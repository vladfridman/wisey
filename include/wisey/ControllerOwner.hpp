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
  
  ControllerOwner(Controller* controller);
  
  ~ControllerOwner();
  
  Controller* getObject() const override;

  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;

  llvm::Function* getDestructorFunction(IRGenerationContext& context) const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void allocateVariable(IRGenerationContext& context, std::string name) const override;

};
  
} /* namespace wisey */

#endif /* ControllerOwner_h */
