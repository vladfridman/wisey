//
//  ModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "wisey/Block.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/ModelFieldDeclaration.hpp"

namespace wisey {
  
/**
 * Represents MODEL definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IStatement {
  const std::string mName;
  std::vector<ModelFieldDeclaration*> mFieldDeclarations;
  std::vector<MethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ModelDefinition(std::string name,
                  std::vector<ModelFieldDeclaration*> fieldDeclarations,
                  std::vector<MethodDeclaration *> methodDeclarations,
                  std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mName(name),
  mFieldDeclarations(fieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ModelDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::map<std::string, Field*> createFields(IRGenerationContext& context,
                                             unsigned long numberOfInterfaces) const;
  
  void createFieldVariables(IRGenerationContext& context,
                            Model* model,
                            std::vector<llvm::Type*>& types) const;
  
  std::vector<Method*> createMethods(IRGenerationContext& context) const;
  
  std::map<std::string, llvm::Function*> generateMethodsIR(IRGenerationContext& context,
                                                           Model* model) const;
  
  std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                            std::vector<llvm::Type*>& types) const;

  
  void processInterfaceMethods(IRGenerationContext& context,
                               Model* model,
                               std::vector<Interface*> interfaces,
                               std::map<std::string, llvm::Function*>& methodFunctionMap,
                               llvm::GlobalVariable* typeListGlobal) const;

  std::vector<std::list<llvm::Constant*>>
    generateInterfaceMapFunctions(IRGenerationContext& context,
                                  Model* model,
                                  std::vector<Interface*> interfaces,
                                  std::map<std::string, llvm::Function*>& methodFunctionMap) const;

  std::vector<std::vector<llvm::Constant*>>
    addUnthunkAndTypeTableInfo(IRGenerationContext& context,
                               Model* model,
                               llvm::GlobalVariable* typeListGlobal,
                               std::vector<std::list<llvm::Constant*>> interfaceMapFunctions) const;
 
  void createVTableGlobal(IRGenerationContext& context,
                          Model* model,
                          std::vector<std::vector<llvm::Constant*>> interfaceVTables) const;
  
  void defineTypeName(IRGenerationContext& context, Model* model) const;
  
  llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context, Model* model) const;
};

} /* namespace wisey */

#endif /* ModelDefinition_h */
