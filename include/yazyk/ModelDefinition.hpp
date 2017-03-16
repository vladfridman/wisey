//
//  ModelDefinition.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/ModelFieldDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents MODEL definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IStatement {
  const std::string mName;
  std::vector<ModelFieldDeclaration*> mFieldDeclarations;
  std::vector<MethodDeclaration*> mMethodDeclarations;
  std::vector<std::string> mInterfaces;
  
public:
  
  ModelDefinition(std::string name,
                  std::vector<ModelFieldDeclaration*> fieldDeclarations,
                  std::vector<MethodDeclaration *> methodDeclarations,
                  std::vector<std::string> interfaces) :
  mName(name),
  mFieldDeclarations(fieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaces(interfaces) { }
  
  ~ModelDefinition();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::map<std::string, ModelField*> createFields(IRGenerationContext& context,
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
                               std::map<std::string, llvm::Function*>& methodFunctionMap) const;

  void defineModelTypeName(IRGenerationContext& context, Model* model) const;
  
  void addTypeInformation(IRGenerationContext& context, Model* model) const;
};

} /* namespace yazyk */

#endif /* ModelDefinition_h */
