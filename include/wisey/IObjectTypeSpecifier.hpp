//
//  IObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectTypeSpecifier_h
#define IObjectTypeSpecifier_h

#include <iostream>

#include "wisey/IObjectType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Reprensents a type specifier for object types such as controller, model or interface
 */
class IObjectTypeSpecifier : public ITypeSpecifier {
    
public:
  
  virtual const IObjectType* getType(IRGenerationContext& context) const = 0;

  /**
   * Prints a given package to a stream
   */
  static void printPackageToStream(std::iostream& stream, std::vector<std::string> package);
  
};
  
} /* namespace wisey */

#endif /* IObjectTypeSpecifier_h */
