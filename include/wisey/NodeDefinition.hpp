//
//  NodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeDefinition_h
#define NodeDefinition_h

#include "wisey/FieldDeclaration.hpp"
#include "wisey/Field.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a node definition which is an object that can have immutable model fields
 * and mutable fields that point to other nodes.
 */
class NodeDefinition : public IGlobalStatement {
  NodeTypeSpecifier* mNodeTypeSpecifier;
  std::vector<FieldDeclaration*> mFixedFieldDeclarations;
  std::vector<FieldDeclaration*> mStateFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  NodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                 std::vector<FieldDeclaration*> fixedFieldDeclarations,
                 std::vector<FieldDeclaration*> stateFieldDeclarations,
                 std::vector<IMethodDeclaration *> methodDeclarations,
                 std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mNodeTypeSpecifier(nodeTypeSpecifier),
  mFixedFieldDeclarations(fixedFieldDeclarations),
  mStateFieldDeclarations(stateFieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~NodeDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::vector<Interface*> processInterfaces(IRGenerationContext& context) const;
  
  std::vector<IMethod*> createMethods(IRGenerationContext& context) const;

  std::vector<Field*> createFixedFields(IRGenerationContext& context,
                                        std::vector<FieldDeclaration*>
                                        declarations,
                                        unsigned long startIndex) const;

  std::vector<Field*> createStateFields(IRGenerationContext& context,
                                        std::vector<FieldDeclaration*>
                                        declarations,
                                        unsigned long startIndex) const;

  void createFieldVariables(IRGenerationContext& context,
                            Node* node,
                            std::vector<llvm::Type*>& types) const;

  void createFieldVariablesForDeclarations(IRGenerationContext& context,
                                           std::vector<FieldDeclaration*> declarations,
                                           Node* node,
                                           std::vector<llvm::Type*>& types) const;

};
  
} /* namespace wisey */

#endif /* NodeDefinition_h */
