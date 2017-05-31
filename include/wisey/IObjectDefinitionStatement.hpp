//
//  IObjectDefinitionStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectDefinitionStatement_h
#define IObjectDefinitionStatement_h

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a controller, model or interface definition statement
 */
class IObjectDefinitionStatement : public IStatement {

public:
  
  /**
   * Generate prototypes of controllers, models and interfaces without any methods or fields
   */
  virtual void prototypeObjects(IRGenerationContext& context) const = 0;
  
  /**
   * Generate method prototypes for all object prototypes that were created using prototypeObjects()
   */
  virtual void prototypeMethods(IRGenerationContext& context) const = 0;

};

/**
 * Represents a list of object definition statements
 */
typedef std::vector<IObjectDefinitionStatement*> ObjectDefinitionStatementList;

} /* namespace wisey */

#endif /* IObjectDefinitionStatement_h */
