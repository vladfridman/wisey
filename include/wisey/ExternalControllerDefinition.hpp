//
//  ExternalControllerDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalControllerDefinition_h
#define ExternalControllerDefinition_h

#include <string>
#include <vector>

#include "wisey/ControllerTypeSpecifierFull.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"

namespace wisey {
  
  /**
   * Represents controller definition implemented in a shared library
   */
  class ExternalControllerDefinition : public IConcreteObjectDefinition {
    ControllerTypeSpecifierFull* mControllerTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ExternalControllerDefinition(ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                                 std::vector<IObjectElementDefinition*> objectElementDeclarations,
                                 std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                 std::vector<IObjectDefinition*> innerObjectDefinitions,
                                 int line);
    
    ~ExternalControllerDefinition();
    
    Controller* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */
#endif /* ExternalControllerDefinition_h */

