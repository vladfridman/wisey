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
#include "wisey/FieldFixed.hpp"
#include "wisey/FieldInjected.hpp"
#include "wisey/FieldState.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a node definition which is an object that can have immutable model fields
 * and mutable fields that point to other nodes.
 */
class NodeDefinition : public IGlobalStatement {
  const std::string mName;
  std::vector<FieldDeclaration*> mFixedFieldDeclarations;
  std::vector<FieldDeclaration*> mStateFieldDeclarations;
  std::vector<IMethodDeclaration*> mMethodDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  NodeDefinition(std::string name,
                 std::vector<FieldDeclaration*> fixedFieldDeclarations,
                 std::vector<FieldDeclaration*> stateFieldDeclarations,
                 std::vector<IMethodDeclaration *> methodDeclarations,
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

  std::vector<FieldFixed*> createFixedFields(IRGenerationContext& context,
                                             std::vector<FieldDeclaration*>
                                             declarations,
                                             unsigned long startIndex) const;
  
  std::vector<FieldState*> createStateFields(IRGenerationContext& context,
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
