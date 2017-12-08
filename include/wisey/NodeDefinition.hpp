//
//  NodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeDefinition_h
#define NodeDefinition_h

#include "wisey/Field.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/NodeTypeSpecifierFull.hpp"

namespace wisey {
  
/**
 * Represents a node definition which is an object that can have immutable model fields
 * and mutable fields that point to other nodes.
 */
class NodeDefinition : public IConcreteObjectDefinition {
  NodeTypeSpecifierFull* mNodeTypeSpecifierFull;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
  std::vector<IObjectDefinition*> mInnerObjectDefinitions;

public:
  
  NodeDefinition(NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                 std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                 std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                 std::vector<IObjectDefinition*> innerObjectDefinitions);
  
  ~NodeDefinition();

  const Node* prototypeObject(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* NodeDefinition_h */
