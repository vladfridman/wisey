//
//  IObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectDefinition_h
#define IObjectDefinition_h

#include "wisey/IGlobalStatement.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {

class IRGenerationContext;
  
/**
 * Represents an object definition such as controller, model or interface definitions
 */
class IObjectDefinition : public IGlobalStatement {
  
public:
  
  /**
   * Returns object's full name derived from its type specifier and outer object definitions
   */
  static std::string getFullName(IRGenerationContext& context, IObjectTypeSpecifier* typeSpecifier);
  
  /**
   * Calls prototypeObject() on inner objects
   */
  static void prototypeInnerObjects(IRGenerationContext& context,
                                    IObjectType* outerObject,
                                    std::vector<IObjectDefinition*> innerObjectDefinitions);

  
  /**
   * Calls prototypeMethods() on inner objects
   */
  static void prototypeInnerObjectMethods(IRGenerationContext& context,
                                          std::vector<IObjectDefinition*> innerObjectDefinitions);

  /**
   * Calls generateIR() on inner objects
   */
  static void generateInnerObjectIR(IRGenerationContext& context,
                                    std::vector<IObjectDefinition*> innerObjectDefinitions);

};
  
} /* namespace wisey */

#endif /* IObjectDefinition_h */
