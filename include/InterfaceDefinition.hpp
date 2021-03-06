//
//  InterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceDefinition_h
#define InterfaceDefinition_h

#include "AccessLevel.hpp"
#include "InterfaceTypeSpecifierFull.hpp"
#include "IObjectDefinition.hpp"
#include "IObjectElementDefinition.hpp"

namespace wisey {
  
  /**
   * Represents an interface definition which is analogous to a pure virtual class in C++
   */
  class InterfaceDefinition : public IObjectDefinition {
    AccessLevel mAccessLevel;
    InterfaceTypeSpecifierFull* mInterfaceTypeSpecifierFull;
    std::vector<IInterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
    std::vector<IObjectElementDefinition *> mElementDeclarations;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    InterfaceDefinition(AccessLevel accessLevel,
                        InterfaceTypeSpecifierFull* interfaceTypeSpecifierFull,
                        std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                        std::vector<IObjectElementDefinition *> elementDeclarations,
                        std::vector<IObjectDefinition*> innerObjectDefinitions,
                        int line);
    
    ~InterfaceDefinition();
    
    Interface* prototypeObject(IRGenerationContext& context,
                               ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* InterfaceDefinition_h */

