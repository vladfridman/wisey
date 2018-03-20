//
//  ExternalInterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalInterfaceDefinition_h
#define ExternalInterfaceDefinition_h

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/InterfaceTypeSpecifierFull.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents interface definition defined in a shared library
   */
  class ExternalInterfaceDefinition : public IConcreteObjectDefinition {
    InterfaceTypeSpecifierFull* mInterfaceTypeSpecifierFull;
    std::vector<IInterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
    std::vector<IObjectElementDefinition*> mElementDeclarations;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    
  public:
    
    ExternalInterfaceDefinition(InterfaceTypeSpecifierFull* interfaceTypeSpecifierFull,
                                std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                std::vector<IObjectElementDefinition*> elementDeclarations,
                                std::vector<IObjectDefinition*> innerObjectDefinitions);
    
    ~ExternalInterfaceDefinition();
    
    Interface* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ExternalInterfaceDefinition_h */

