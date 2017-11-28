//
//  BindAction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BindAction_h
#define BindAction_h

#include <string>

#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a bind action binding a controller to an interface
 */
class BindAction : public IGlobalStatement {
  ControllerTypeSpecifier* mContreollerTypeSpecifier;
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  
public:
  
  BindAction(ControllerTypeSpecifier* controllerTypeSpecifier,
             InterfaceTypeSpecifier* interfaceTypeSpecifier);
  
  ~BindAction();

  void prototypeObjects(IRGenerationContext& context) const override;

  void prototypeMethods(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* BindAction_h */
