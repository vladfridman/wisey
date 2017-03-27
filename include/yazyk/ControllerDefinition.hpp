//
//  ControllerDefinition.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerDefinition_h
#define ControllerDefinition_h

#include <string>
#include <vector>

#include "yazyk/ControllerFieldDeclaration.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/MethodDeclaration.hpp"

namespace yazyk {

/**
 * Represents CONTROLLER definition which is analogous to a class in C++ with dependency injection
 */
class ControllerDefinition : public IStatement {
  std::string mName;
  std::vector<ControllerFieldDeclaration*> mReceivedFeildDeclarations;
  std::vector<ControllerFieldDeclaration*> mInjectedFeildDeclarations;
  std::vector<ControllerFieldDeclaration*> mStateFeildDeclarations;
  std::vector<MethodDeclaration*> mMethodDeclarations;
  std::vector<std::string> mInterfaces;
  
public:
  
  ControllerDefinition(std::string name,
                       std::vector<ControllerFieldDeclaration*> receivedFieldDeclarations,
                       std::vector<ControllerFieldDeclaration*> injectedFieldDeclarations,
                       std::vector<ControllerFieldDeclaration*> stateFieldDeclarations,
                       std::vector<MethodDeclaration*> methodDeclarations,
                       std::vector<std::string> interfaces) :
  mName(name),
  mReceivedFeildDeclarations(receivedFieldDeclarations),
  mInjectedFeildDeclarations(injectedFieldDeclarations),
  mStateFeildDeclarations(stateFieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaces(interfaces) { }
  
  ~ControllerDefinition() { };
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};

} /* namespace yazyk */

#endif /* ControllerDefinition_h */
