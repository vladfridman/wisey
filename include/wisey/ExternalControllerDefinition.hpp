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

#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents controller definition implemented in a shared library
 */
class ExternalControllerDefinition : public IConcreteObjectDefinition {
  ControllerTypeSpecifier* mControllerTypeSpecifier;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalControllerDefinition(ControllerTypeSpecifier* controllerTypeSpecifier,
                               std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                               std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ExternalControllerDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */
#endif /* ExternalControllerDefinition_h */
