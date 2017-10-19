//
//  IObjectElement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectElement_h
#define IObjectElement_h

namespace wisey {
  
/**
 * An object element can be either a field or a method
 */
typedef enum ObjectElementTypeEnum {
  OBJECT_ELEMENT_FIELD,
  OBJECT_ELEMENT_METHOD,
} ObjectElementType;

/**
 * Represents a an object element such as a method or a field
 */
class IObjectElement {
  
public:
  
  virtual ~IObjectElement() { }
  
  virtual ObjectElementType getObjectElementType() const = 0;

};

} /* namespace wisey */

#endif /* IObjectElement_h */
