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
  
  Controller* getObjectType() const override;

  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;
  
  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isPrimitive() const override;

  bool isOwner() const override;
  
  bool isReference() const override;
  
  bool isArray() const override;
  
  bool isFunction() const override;
  
  bool isPackage() const override;

  bool isController() const override;
  
  bool isInterface() const override;
  
  bool isModel() const override;
  
  bool isNode() const override;
  
  bool isThread() const override;
  
  bool isNative() const override;

  llvm::Function* getDestructorFunction(IRGenerationContext& context) const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void createLocalVariable(IRGenerationContext& context, std::string name) const override;
  
  void createFieldVariable(IRGenerationContext& context,
                           std::string name,
                           const IConcreteObjectType* object) const override;
  
  void createParameterVariable(IRGenerationContext& context,
                               std::string name,
                               llvm::Value* value) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* ControllerOwner_h */
