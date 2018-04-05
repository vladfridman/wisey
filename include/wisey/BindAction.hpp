//
//  BindAction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BindAction_h
#define BindAction_h

#include "wisey/IControllerTypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a bind action binding a controller to an interface with given injection arguments
   */
  class BindAction {
    IInterfaceTypeSpecifier* mInterfaceTypeSpecifier;
    IControllerTypeSpecifier* mContreollerTypeSpecifier;
    InjectionArgumentList mInjectionArguments;
    
  public:
    
    BindAction(IInterfaceTypeSpecifier* interfaceTypeSpecifier,
               IControllerTypeSpecifier* controllerTypeSpecifier,
               InjectionArgumentList injectionArguments);
    
    ~BindAction();
    
    const Interface* getInterface(IRGenerationContext& context) const;
    
    const Controller* getController(IRGenerationContext& context) const;
    
    InjectionArgumentList getInjectionArguments(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* BindAction_h */

