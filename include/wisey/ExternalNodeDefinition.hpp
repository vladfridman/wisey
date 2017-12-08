//
//  ExternalNodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalNodeDefinition_h
#define ExternalNodeDefinition_h

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/NodeTypeSpecifierFull.hpp"

namespace wisey {
  
/**
 * Represents node definition implemented in a shared library
 */
class ExternalNodeDefinition : public IConcreteObjectDefinition {
  NodeTypeSpecifierFull* mNodeTypeSpecifierFull;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
  std::vector<IObjectDefinition*> mInnerObjectDefinitions;

public:
  
  ExternalNodeDefinition(NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                         std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                         std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                         std::vector<IObjectDefinition*> innerObjectDefinitions);
  
  ~ExternalNodeDefinition();
  
  const Node* prototypeObject(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */


#endif /* ExternalNodeDefinition_h */
