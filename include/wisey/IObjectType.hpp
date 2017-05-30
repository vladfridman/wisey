//
//  IObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectType_h
#define IObjectType_h

#include "wisey/IType.hpp"
#include "wisey/IMethodDescriptor.hpp"

namespace wisey {
  
/**
 * Represents an IType that is either model, controller or interface
 */
class IObjectType : public IType {
    
public:
  
  /**
   * Returns short name of the object without the package name.
   */
  virtual std::string getShortName() const = 0;

  /**
   * Finds a method with a given name.
   */
  virtual IMethodDescriptor* findMethod(std::string methodName) const = 0;
  
  /**
   * Returns name of the global variable containing name of this Callable Object in string format
   */
  virtual std::string getObjectNameGlobalVariableName() const = 0;
  
  /**
   * Returns an i8* constant pointer to the name of the collable object
   */
  static llvm::Constant* getObjectNamePointer(const IObjectType* object,
                                              IRGenerationContext& context);
};
  
} /* namespace wisey */

#endif /* IObjectType_h */
