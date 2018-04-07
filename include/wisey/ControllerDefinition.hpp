//
//  ControllerDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerDefinition_h
#define ControllerDefinition_h

#include <string>
#include <vector>

#include "wisey/AccessLevel.hpp"
#include "wisey/ControllerTypeSpecifierFull.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"

namespace wisey {
  
  /**
   * Represents controller definition which is analogous to a class in C++ with dependency injection
   */
  class ControllerDefinition : public IConcreteObjectDefinition {
    AccessLevel mAccessLevel;
    ControllerTypeSpecifierFull* mControllerTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ControllerDefinition(AccessLevel accessLevel,
                         ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                         std::vector<IObjectElementDefinition*> objectElementDeclarations,
                         std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                         std::vector<IObjectDefinition*> innerObjectDefinitions,
                         int line);
    
    ~ControllerDefinition();
    
    Controller* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ControllerDefinition_h */

