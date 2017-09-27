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
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FieldInjected.hpp"
#include "wisey/FieldReceived.hpp"
#include "wisey/FieldState.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

namespace wisey {
  
/**
 * Represents controller definition implemented in a shared library
 */
class ExternalControllerDefinition : public IGlobalStatement {
  ControllerTypeSpecifier* mControllerTypeSpecifier;
  std::vector<FieldDeclaration*> mReceivedFieldDeclarations;
  std::vector<FieldDeclaration*> mInjectedFieldDeclarations;
  std::vector<FieldDeclaration*> mStateFieldDeclarations;
  std::vector<MethodSignatureDeclaration*> mMethodSignatures;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalControllerDefinition(ControllerTypeSpecifier* controllerTypeSpecifier,
                               std::vector<FieldDeclaration*> receivedFieldDeclarations,
                               std::vector<FieldDeclaration*> injectedFieldDeclarations,
                               std::vector<FieldDeclaration*> stateFieldDeclarations,
                               std::vector<MethodSignatureDeclaration*> methodSignatures,
                               std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mControllerTypeSpecifier(controllerTypeSpecifier),
  mReceivedFieldDeclarations(receivedFieldDeclarations),
  mInjectedFieldDeclarations(injectedFieldDeclarations),
  mStateFieldDeclarations(stateFieldDeclarations),
  mMethodSignatures(methodSignatures),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ExternalControllerDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */
#endif /* ExternalControllerDefinition_h */
