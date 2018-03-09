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
 * Represents a an object element such as a method or a field
 */
class IObjectElement {
  
public:
  
  virtual ~IObjectElement() { }

  /**
   * Methods for determining the type of object element
   */
  virtual bool isConstant() const = 0;
  
  virtual bool isField() const = 0;
  
  virtual bool isMethod() const = 0;
  
  virtual bool isStaticMethod() const = 0;
  
  virtual bool isMethodSignature() const = 0;

};

} /* namespace wisey */

#endif /* IObjectElement_h */
