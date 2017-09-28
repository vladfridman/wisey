//
//  ExternalNodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalNodeDefinition_h
#define ExternalNodeDefinition_h

#include "wisey/FieldDeclaration.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents node definition implemented in a shared library
 */
class ExternalNodeDefinition : public IConcreteObjectDefinition {
  NodeTypeSpecifier* mNodeTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<MethodSignatureDeclaration*> mMethodSignatures;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalNodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                         std::vector<FieldDeclaration*> fieldDeclarations,
                         std::vector<MethodSignatureDeclaration *> methodSignatures,
                         std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mNodeTypeSpecifier(nodeTypeSpecifier),
  mFieldDeclarations(fieldDeclarations),
  mMethodSignatures(methodSignatures),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ExternalNodeDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */


#endif /* ExternalNodeDefinition_h */
