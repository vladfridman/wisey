//
//  IStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IStatement_h
#define IStatement_h

#include "wisey/INode.hpp"

namespace wisey {

/**
 * Interface representing a wisey language statement
 */
class IStatement : public INode {

public:

  /**
   * Generate prototypes of controllers, models and interfaces and their methods
   */
  virtual void prototype(IRGenerationContext& context) const = 0;
};

/**
 * Represents a list of statements
 */
typedef std::vector<IStatement*> StatementList;

} /* namespace wisey */

#endif /* IStatement_h */
