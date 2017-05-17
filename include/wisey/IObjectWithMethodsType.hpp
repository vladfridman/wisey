//
//  IObjectWithMethodsType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectWithMethodsType_h
#define IObjectWithMethodsType_h

#include "wisey/IType.hpp"

namespace yazyk {
  
class IMethodDescriptor;
  
/**
 * Represents an object that has methods such as a Model or an Interface
 */
class IObjectWithMethodsType : public virtual IType {
  
public:
  
  /**
   * Finds a method with a given name
   */
  virtual IMethodDescriptor* findMethod(std::string methodName) const = 0;

  /**
   * Returns name of the global variable containing name of this Callable Object in string format
   */
  virtual std::string getObjectNameGlobalVariableName() const = 0;
  
  /**
   * Returns an i8* constant pointer to the name of the collable object
   */
  static llvm::Constant* getObjectNamePointer(const IObjectWithMethodsType* object,
                                              IRGenerationContext& context);
};

} /* namespace yazyk */

#endif /* IObjectWithMethodsType_h */
