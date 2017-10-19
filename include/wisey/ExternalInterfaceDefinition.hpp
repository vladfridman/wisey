//
//  ExternalInterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalInterfaceDefinition_h
#define ExternalInterfaceDefinition_h

#include "wisey/FieldDeclaration.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents interface definition defined in a shared library
 */
class ExternalInterfaceDefinition : public IConcreteObjectDefinition {
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  std::vector<InterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<IObjectElementDeclaration*> mElementDeclarations;

public:
  
  ExternalInterfaceDefinition(InterfaceTypeSpecifier* interfaceTypeSpecifier,
                              std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                              std::vector<IObjectElementDeclaration*> elementDeclarations);
  
  ~ExternalInterfaceDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ExternalInterfaceDefinition_h */
