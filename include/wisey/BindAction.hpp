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
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a bind action binding a controller to an interface
 */
class BindAction : public IStatement {
  ControllerTypeSpecifier* mContreollerTypeSpecifier;
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  
public:
  
  BindAction(ControllerTypeSpecifier* controllerTypeSpecifier,
             InterfaceTypeSpecifier* interfaceTypeSpecifier)
  : mContreollerTypeSpecifier(controllerTypeSpecifier),
  mInterfaceTypeSpecifier(interfaceTypeSpecifier) { }
  
  ~BindAction();
 
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* BindAction_h */
