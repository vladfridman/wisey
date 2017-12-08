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
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"

namespace wisey {
  
/**
 * Represents model definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IConcreteObjectDefinition {
  ModelTypeSpecifierFull* mModelTypeSpecifierFull;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
  std::vector<IObjectDefinition*> mInnerObjectDefinitions;

public:
  
  ModelDefinition(ModelTypeSpecifierFull* modelTypeSpecifierFull,
                  std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                  std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                  std::vector<IObjectDefinition*> innerObjectDefinitions);
  
  ~ModelDefinition();

  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};

} /* namespace wisey */

#endif /* ModelDefinition_h */
