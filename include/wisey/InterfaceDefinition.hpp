//
//  InterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceDefinition_h
#define InterfaceDefinition_h

#include "wisey/Block.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

namespace wisey {
  
/**
 * Represents an INTERFACE definition which is analogous to a pure virtual class in C++
 */
class InterfaceDefinition : public IGlobalStatement {
  const std::string mName;
  std::vector<InterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<MethodSignatureDeclaration *> mMethodSignatureDeclarations;
  
public:
  
  InterfaceDefinition(std::string name,
                      std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                      std::vector<MethodSignatureDeclaration *> methodSignatureDeclarations) :
  mName(name),
  mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
  mMethodSignatureDeclarations(methodSignatureDeclarations) { }
  
  ~InterfaceDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  void defineInterfaceTypeName(IRGenerationContext& context, Interface* interface) const;
  
  std::string getFullName(IRGenerationContext& context) const;

};
  
} /* namespace wisey */

#endif /* InterfaceDefinition_h */
