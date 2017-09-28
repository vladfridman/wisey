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
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents controller definition which is analogous to a class in C++ with dependency injection
 */
class ControllerDefinition : public IGlobalStatement {
  ControllerTypeSpecifier* mControllerTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ControllerDefinition(ControllerTypeSpecifier* controllerTypeSpecifier,
                       std::vector<FieldDeclaration*> fieldDeclarations,
                       std::vector<IMethodDeclaration*> methodDeclarations,
                       std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mControllerTypeSpecifier(controllerTypeSpecifier),
  mFieldDeclarations(fieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ControllerDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:
  
  std::vector<Interface*> processInterfaces(IRGenerationContext& context) const;

  std::vector<Field*> createFields(IRGenerationContext& context, unsigned long startIndex) const;

  std::vector<IMethod*> createMethods(IRGenerationContext& context) const;

  void collectFieldTypes(IRGenerationContext& context,
                         Controller* controller,
                         std::vector<llvm::Type*>& types) const;

};

} /* namespace wisey */

#endif /* ControllerDefinition_h */
