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

#include "wisey/ControllerFieldDeclaration.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/MethodDeclaration.hpp"

namespace wisey {

/**
 * Represents CONTROLLER definition which is analogous to a class in C++ with dependency injection
 */
class ControllerDefinition : public IStatement {
  std::string mName;
  std::vector<ControllerFieldDeclaration*> mReceivedFieldDeclarations;
  std::vector<ControllerFieldDeclaration*> mInjectedFieldDeclarations;
  std::vector<ControllerFieldDeclaration*> mStateFieldDeclarations;
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
  mReceivedFieldDeclarations(receivedFieldDeclarations),
  mInjectedFieldDeclarations(injectedFieldDeclarations),
  mStateFieldDeclarations(stateFieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaces(interfaces) { }
  
  ~ControllerDefinition();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:
  
  std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                            std::vector<llvm::Type*>& types) const;

  std::vector<Field*> fieldDeclarationsToFields(IRGenerationContext& context,
                                                std::vector<ControllerFieldDeclaration*>
                                                  declarations,
                                                unsigned long startIndex) const;

  std::vector<Method*> createMethods(IRGenerationContext& context) const;

  void createFieldVariables(IRGenerationContext& context,
                            Controller* controller,
                            std::vector<llvm::Type*>& types) const;

  void createFieldVariablesForDeclarations(IRGenerationContext& context,
                                           std::vector<ControllerFieldDeclaration*> declarations,
                                           Controller* controller,
                                           std::vector<llvm::Type*>& types) const;

  std::map<std::string, llvm::Function*> generateMethodsIR(IRGenerationContext& context,
                                                           Controller* controller) const;

  void defineTypeName(IRGenerationContext& context, Controller* controller) const;

  llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context,
                                             Controller* controller) const;

  void processInterfaceMethods(IRGenerationContext& context,
                               Controller* controller,
                               std::vector<Interface*> interfaces,
                               std::map<std::string, llvm::Function*>& methodFunctionMap,
                               llvm::GlobalVariable* typeListGlobal) const;

  std::vector<std::list<llvm::Constant*>>
  generateInterfaceMapFunctions(IRGenerationContext& context,
                                Controller* controller,
                                std::vector<Interface*> interfaces,
                                std::map<std::string, llvm::Function*>& methodFunctionMap) const;

  std::vector<std::vector<llvm::Constant*>>
  addUnthunkAndTypeTableInfo(IRGenerationContext& context,
                             Controller* controller,
                             llvm::GlobalVariable* typeListGlobal,
                             std::vector<std::list<llvm::Constant*>> interfaceMapFunctions) const;

  void createVTableGlobal(IRGenerationContext& context,
                          Controller* controller,
                          std::vector<std::vector<llvm::Constant*>> interfaceVTables) const;
};

} /* namespace wisey */

#endif /* ControllerDefinition_h */
