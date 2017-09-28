//
//  ModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "wisey/Field.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents model definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IGlobalStatement {
  ModelTypeSpecifier* mModelTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                  std::vector<FieldDeclaration*> fieldDeclarations,
                  std::vector<IMethodDeclaration *> methodDeclarations,
                  std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mModelTypeSpecifier(modelTypeSpecifier),
  mFieldDeclarations(fieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ModelDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::vector<Field*> createFields(IRGenerationContext& context,
                                   unsigned long numberOfInterfaces) const;
  
  void collectFieldTypes(IRGenerationContext& context,
                         Model* model,
                         std::vector<llvm::Type*>& types) const;

  std::vector<IMethod*> createMethods(IRGenerationContext& context) const;
    
  std::vector<Interface*> processInterfaces(IRGenerationContext& context) const;

};

} /* namespace wisey */

#endif /* ModelDefinition_h */
