//
//  IConcreteObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectDefinition_h
#define IConcreteObjectDefinition_h

#include "wisey/FieldDeclaration.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IObjectDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
class IRGenerationContext;
  
/**
 * Represents a concrete object definition such as controller, model or node definitions
 */
class IConcreteObjectDefinition : public IObjectDefinition {
  
public:
  
  /**
   * Fill up concrete object prototype with methods, fields, types and other data
   */
  static void configureConcreteObject(IRGenerationContext& context,
                                      IConcreteObjectType* object,
                                      std::vector<FieldDeclaration*> fieldDeclarations,
                                      std::vector<IMethodDeclaration*> methodDeclarations,
                                      std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);

private:

  static std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                                   std::vector<InterfaceTypeSpecifier*>
                                                   interfaceSpecifiers);
  
  static std::vector<IMethod*> createMethods(IRGenerationContext& context,
                                             std::vector<IMethodDeclaration*> methodDeclarations);
  
  static void collectFieldTypes(IRGenerationContext& context,
                                std::vector<llvm::Type*>& types,
                                std::vector<FieldDeclaration*> fieldDeclarations);

};

} /* namespace wisey */

#endif /* IConcreteObjectDefinition_h */
