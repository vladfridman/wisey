//
//  IObjectWithFieldsType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectWithFieldsType_h
#define IObjectWithFieldsType_h

#include "wisey/IType.hpp"

namespace wisey {

class Field;
  
/**
 * Represents an IType of an object with fields such as a Model or a Controller
 */
class IObjectWithFieldsType : public virtual IType {
    
public:
  
  /**
   * Looks for a field with a given name in the object
   */
  virtual Field* findField(std::string fieldName) const = 0;

};
  
} /* namespace wisey */

#endif /* IObjectWithFieldsType_h */
