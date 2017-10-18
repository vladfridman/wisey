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
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents controller definition implemented in a shared library
 */
class ExternalControllerDefinition : public IConcreteObjectDefinition {
  ControllerTypeSpecifier* mControllerTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalControllerDefinition(ControllerTypeSpecifier* controllerTypeSpecifier,
                               std::vector<FieldDeclaration*> fieldDeclarations,
                               std::vector<IMethodDeclaration*> methodDeclarations,
                               std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ExternalControllerDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */
#endif /* ExternalControllerDefinition_h */
