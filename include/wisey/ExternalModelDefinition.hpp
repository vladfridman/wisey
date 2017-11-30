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
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents model definition implemented in a shared library
 */
class ExternalModelDefinition : public IConcreteObjectDefinition {
  IModelTypeSpecifier* mModelTypeSpecifier;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalModelDefinition(IModelTypeSpecifier* modelTypeSpecifier,
                          std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                          std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ExternalModelDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
};

} /* namespace wisey */

#endif /* ExternalModelDefinition_h */
