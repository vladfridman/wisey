//
//  IConcreteObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectDefinition_h
#define IConcreteObjectDefinition_h

#include "wisey/Constant.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
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
   *
   * The llvm struct representing the object consists of
   * {
   *    int8* imlemented interface1 vtable pointer,
   *    int8* imlemented interface2 vtable pointer,
   *    ...
   *    int64 reference pointer counter,
   *    <field1 type> field1,
   *    <field2 type> field2,
   * }
   */
  static void configureObject(IRGenerationContext& context,
                              IConcreteObjectType* object,
                              std::vector<IObjectElementDeclaration*> elementDeclarations,
                              std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
private:

  static std::tuple<std::vector<Constant*>, std::vector<Field*>, std::vector<IMethod*>>
  createElements(IRGenerationContext& context,
                 std::vector<IObjectElementDeclaration*>
                 elementDeclarations);

  static std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                                   std::vector<InterfaceTypeSpecifier*>
                                                   interfaceSpecifiers);
  
  static void collectFieldTypes(IRGenerationContext& context,
                                std::vector<llvm::Type*>& types,
                                std::vector<Field*> fields);

};

} /* namespace wisey */

#endif /* IConcreteObjectDefinition_h */
