//
//  IConcreteObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectDefinition_h
#define IConcreteObjectDefinition_h

#include "Constant.hpp"
#include "IConcreteObjectType.hpp"
#include "IField.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IObjectElement.hpp"
#include "IObjectElementDefinition.hpp"
#include "IObjectDefinition.hpp"

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
     *    int64 reference pointer counter,
     *    int8* imlemented interface1 vtable pointer,
     *    int8* imlemented interface2 vtable pointer,
     *    ...
     *    <field1 type> field1,
     *    <field2 type> field2,
     *    ...
     * }
     */
    static void configureObject(IRGenerationContext& context,
                                IConcreteObjectType* object,
                                std::vector<IObjectElementDefinition*> elementDeclarations,
                                std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                const IObjectTypeSpecifier* scopeObjectSpecifier);
    
  private:
    
    static std::tuple<std::vector<Constant*>, std::vector<IField*>, std::vector<IMethod*>,
    std::vector<LLVMFunction*>> createElements(IRGenerationContext& context,
                                               const IConcreteObjectType* concreteObjectType,
                                               std::vector<IObjectElementDefinition*>
                                               elementDeclarations);

    static std::vector<Interface*> processInterfaces(IRGenerationContext& context,
                                                     const IConcreteObjectType* concreteObjectType,
                                                     std::vector<IInterfaceTypeSpecifier*>
                                                     interfaceSpecifiers);
    
    static void collectFieldTypes(IRGenerationContext& context,
                                  std::vector<llvm::Type*>& types,
                                  std::vector<IField*> fields);
    
    static void maybeAddPoolStore(IRGenerationContext& context,
                                  std::vector<llvm::Type*>& types,
                                  const IConcreteObjectType* concreteObjectType);

  };
  
} /* namespace wisey */

#endif /* IConcreteObjectDefinition_h */

