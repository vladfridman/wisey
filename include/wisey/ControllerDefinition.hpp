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

#include "wisey/FieldDeclaration.hpp"
#include "wisey/FieldInjected.hpp"
#include "wisey/FieldReceived.hpp"
#include "wisey/FieldState.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"

namespace wisey {

/**
 * Represents CONTROLLER definition which is analogous to a class in C++ with dependency injection
 */
class ControllerDefinition : public IGlobalStatement {
  std::string mName;
  std::vector<FieldDeclaration*> mReceivedFieldDeclarations;
  std::vector<FieldDeclaration*> mInjectedFieldDeclarations;
  std::vector<FieldDeclaration*> mStateFieldDeclarations;
  std::vector<MethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ControllerDefinition(std::string name,
                       std::vector<FieldDeclaration*> receivedFieldDeclarations,
                       std::vector<FieldDeclaration*> injectedFieldDeclarations,
                       std::vector<FieldDeclaration*> stateFieldDeclarations,
                       std::vector<MethodDeclaration*> methodDeclarations,
                       std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mName(name),
  mReceivedFieldDeclarations(receivedFieldDeclarations),
  mInjectedFieldDeclarations(injectedFieldDeclarations),
  mStateFieldDeclarations(stateFieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ControllerDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:
  
  std::vector<Interface*> processInterfaces(IRGenerationContext& context) const;

  std::vector<FieldReceived*> createReceivedFields(IRGenerationContext& context,
                                                   std::vector<FieldDeclaration*> declarations,
                                                   unsigned long startIndex) const;

  std::vector<FieldInjected*> createInjectedFields(IRGenerationContext& context,
                                                   std::vector<FieldDeclaration*> declarations,
                                                   unsigned long startIndex) const;

  std::vector<FieldState*> createStateFields(IRGenerationContext& context,
                                                std::vector<FieldDeclaration*> declarations,
                                                unsigned long startIndex) const;

  std::vector<Method*> createMethods(IRGenerationContext& context) const;

  void createFieldVariables(IRGenerationContext& context,
                            Controller* controller,
                            std::vector<llvm::Type*>& types) const;

  void createFieldVariablesForDeclarations(IRGenerationContext& context,
                                           std::vector<FieldDeclaration*> declarations,
                                           Controller* controller,
                                           std::vector<llvm::Type*>& types) const;

  std::string getFullName(IRGenerationContext& context) const;
};

} /* namespace wisey */

#endif /* ControllerDefinition_h */
