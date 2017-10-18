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
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents model definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IConcreteObjectDefinition {
  ModelTypeSpecifier* mModelTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                  std::vector<FieldDeclaration*> fieldDeclarations,
                  std::vector<IMethodDeclaration *> methodDeclarations,
                  std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ModelDefinition();

  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};

} /* namespace wisey */

#endif /* ModelDefinition_h */
