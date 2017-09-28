//
//  IExternalObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IExternalObjectDefinition_h
#define IExternalObjectDefinition_h

#include "wisey/FieldDeclaration.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

namespace wisey {
  
class IRGenerationContext;
  
/**
 * Represents an external object definition, an object whose implementation is in a library
 */
class IExternalObjectDefinition : public IConcreteObjectDefinition {
  
public:
  
  /**
   * Fill up external object prototype with methods, fields, types and other data
   */
  static void configureExternalObject(IRGenerationContext& context,
                                      IConcreteObjectType* object,
                                      std::vector<FieldDeclaration*> fieldDeclarations,
                                      std::vector<MethodSignatureDeclaration*> methodSignatures,
                                      std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);

private:
  
  static std::vector<IMethod*> createExternalMethods(IRGenerationContext& context,
                                                     std::vector<MethodSignatureDeclaration*>
                                                     methodSignatures);

};
  
} /* namespace wisey */

#endif /* IExternalObjectDefinition_h */
