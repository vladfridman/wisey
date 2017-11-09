//
//  IObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectType_h
#define IObjectType_h

#include <iostream>

#include "wisey/IType.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IPrintable.hpp"

namespace wisey {

class Constant;
  
/**
 * Represents an IType that is either model, controller or interface
 */
class IObjectType : public IPrintable, public IType {
    
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
   * Finds a method with a given name.
   */
  virtual Constant* findConstant(std::string constantName) const = 0;

  /**
   * Returns name of the global variable containing name of this Callable Object in string format
   */
  virtual std::string getObjectNameGlobalVariableName() const = 0;
  
  /**
   * Returns the owner type for this object
   */
  virtual const IObjectOwnerType* getOwner() const = 0;
  
  /**
   * Tells whether this object is externally implemented in a shared library
   */
  virtual bool isExternal() const = 0;

  /**
   * Override method from IType because ObjectOwner llvm type is always a PointerType
   */
  virtual llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override = 0;
  
  /**
   * Increments reference counter for this object
   */
  virtual void incremenetReferenceCount(IRGenerationContext& context,
                                        llvm::Value* object) const = 0;
  
  /**
   * Decrements reference counter for this object
   */
  virtual void decremenetReferenceCount(IRGenerationContext& context,
                                        llvm::Value* object) const = 0;

  /**
   * Returns pointer reference counter value for this object
   */
  virtual llvm::Value* getReferenceCount(IRGenerationContext& context,
                                         llvm::Value* object) const = 0;

  /**
   * Returns an i8* constant pointer to the name of the collable object
   */
  static llvm::Constant* getObjectNamePointer(const IObjectType* object,
                                              IRGenerationContext& context);

  /**
   * Increments reference counter for the given object in a thread unsafe way
   */
  static void incrementReferenceCounterForObject(IRGenerationContext& context, llvm::Value* object);

  /**
   * Decrements reference counter for the given object in a thread unsafe way
   */
  static void decrementReferenceCounterForObject(IRGenerationContext& context, llvm::Value* object);

  /**
   * Returns the reference count value for the given object
   */
  static llvm::Value* getReferenceCountForObject(IRGenerationContext& context, llvm::Value* object);

private:
  
  static llvm::Value* getReferenceCounterPointer(IRGenerationContext& context, llvm::Value* object);

  static void adjustReferenceCounterForObject(IRGenerationContext& context,
                                              llvm::Value* object,
                                              int adjustment);

};
  
} /* namespace wisey */

#endif /* IObjectType_h */
