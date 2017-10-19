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

#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents controller definition which is analogous to a class in C++ with dependency injection
 */
class ControllerDefinition : public IConcreteObjectDefinition {
  ControllerTypeSpecifier* mControllerTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ControllerDefinition(ControllerTypeSpecifier* controllerTypeSpecifier,
                       std::vector<FieldDeclaration*> fieldDeclarations,
                       std::vector<IMethodDeclaration*> methodDeclarations,
                       std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ControllerDefinition();

  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;

};

} /* namespace wisey */

#endif /* ControllerDefinition_h */
