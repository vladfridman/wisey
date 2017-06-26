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
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"

namespace wisey {
  
/**
 * Represents a node definition which is an object that can have immutable model fields
 * and mutable fields that point to other nodes.
 */
class NodeDefinition : public IGlobalStatement {
  const std::string mName;
  std::vector<FieldDeclaration*> mFixedFieldDeclarations;
  std::vector<FieldDeclaration*> mStateFieldDeclarations;
  std::vector<MethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  NodeDefinition(std::string name,
                 std::vector<FieldDeclaration*> fixedFieldDeclarations,
                 std::vector<FieldDeclaration*> stateFieldDeclarations,
                 std::vector<MethodDeclaration *> methodDeclarations,
                 std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mName(name),
  mFixedFieldDeclarations(fixedFieldDeclarations),
  mStateFieldDeclarations(stateFieldDeclarations),
  mMethodDeclarations(methodDeclarations),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~NodeDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::string getFullName(IRGenerationContext& context) const;

  std::vector<Interface*> processInterfaces(IRGenerationContext& context) const;
  
  std::vector<Method*> createMethods(IRGenerationContext& context) const;

  std::vector<Field*> fieldDeclarationsToFields(IRGenerationContext& context,
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
