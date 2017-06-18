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
#include "wisey/IObjectOwnerType.hpp"

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
   * Returns the owner type for this object
   */
  virtual IObjectOwnerType* getOwner() const = 0;
  
  /**
   * Override method from IType because ObjectOwner llvm type is always a PointerType
   */
  virtual llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override = 0;

  /**
   * Returns an i8* constant pointer to the name of the collable object
   */
  static llvm::Constant* getObjectNamePointer(const IObjectType* object,
                                              IRGenerationContext& context);
};
  
} /* namespace wisey */

#endif /* IObjectType_h */
