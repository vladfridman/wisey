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
  static void configureObject(IRGenerationContext& context,
                              IConcreteObjectType* object,
                              std::vector<FieldDeclaration*> fieldDeclarations,
                              std::vector<IMethodDeclaration*> methodDeclarations,
                              std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);

  /**
   * Create fields given field declarations
   */
  static std::vector<Field*> createFields(IRGenerationContext& context,
                                          std::vector<FieldDeclaration*> fieldDeclarations,
                                          unsigned long startIndex);
 
  /**
   * Create list of interface pointers given interface type specifiers
   */
  static std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                                   std::vector<InterfaceTypeSpecifier*>
                                                   interfaceSpecifiers);
  
  /**
   * Goes through field declarations and collects llvm types for subsequent object struct definition
   */
  static void collectFieldTypes(IRGenerationContext& context,
                                std::vector<llvm::Type*>& types,
                                std::vector<Field*> fields);
  
private:

  static std::vector<IMethod*> createMethods(IRGenerationContext& context,
                                             std::vector<IMethodDeclaration*> methodDeclarations);
  
};

} /* namespace wisey */

#endif /* IConcreteObjectDefinition_h */
