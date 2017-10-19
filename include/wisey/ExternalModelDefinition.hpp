//
//  ExternalModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalModelDefinition_h
#define ExternalModelDefinition_h

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents model definition implemented in a shared library
 */
class ExternalModelDefinition : public IConcreteObjectDefinition {
  ModelTypeSpecifier* mModelTypeSpecifier;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                          std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                          std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ExternalModelDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
};

} /* namespace wisey */

#endif /* ExternalModelDefinition_h */
