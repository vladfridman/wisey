//
//  ExternalInterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalInterfaceDefinition_h
#define ExternalInterfaceDefinition_h

#include "IConcreteObjectDefinition.hpp"
#include "InterfaceTypeSpecifierFull.hpp"
#include "IObjectElementDefinition.hpp"
#include "ModelTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents interface definition defined in a shared library
   */
  class ExternalInterfaceDefinition : public IConcreteObjectDefinition {
    InterfaceTypeSpecifierFull* mInterfaceTypeSpecifierFull;
    std::vector<IInterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
    std::vector<IObjectElementDefinition*> mElementDeclarations;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ExternalInterfaceDefinition(InterfaceTypeSpecifierFull* interfaceTypeSpecifierFull,
                                std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                std::vector<IObjectElementDefinition*> elementDeclarations,
                                std::vector<IObjectDefinition*> innerObjectDefinitions,
                                int line);
    
    ~ExternalInterfaceDefinition();
    
    Interface* prototypeObject(IRGenerationContext& context,
                               ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ExternalInterfaceDefinition_h */

