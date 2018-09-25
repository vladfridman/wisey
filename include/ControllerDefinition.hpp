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

#include "AccessLevel.hpp"
#include "ControllerTypeSpecifierFull.hpp"
#include "IConcreteObjectDefinition.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IObjectElementDefinition.hpp"

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
    const IObjectTypeSpecifier* mScopeTypeSpecifier;
    int mLine;
    
  public:
    
    ControllerDefinition(AccessLevel accessLevel,
                         ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                         std::vector<IObjectElementDefinition*> objectElementDeclarations,
                         std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                         std::vector<IObjectDefinition*> innerObjectDefinitions,
                         const IObjectTypeSpecifier* scopeTypeSpecifier,
                         int line);
    
    ~ControllerDefinition();
    
    Controller* prototypeObject(IRGenerationContext& context,
                                ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ControllerDefinition_h */

