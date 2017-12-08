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
#include "wisey/ImportProfile.hpp"

namespace wisey {

class Constant;
  
/**
 * Represents an IType that is either model, controller or interface
 */
class IObjectType : public IPrintable, public IType {
    
public:
  
  /**
   * Returns access level for this object
   */
  virtual AccessLevel getAccessLevel() const = 0;
  
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
   * Sets the import profile associated with this object
   */
  virtual void setImportProfile(ImportProfile* importProfile) = 0;

  /**
   * Returns the import profile associated with this object
   */
  virtual ImportProfile* getImportProfile() const = 0;
  
  /**
   * Adds an inner object defined with this object
   */
  virtual void addInnerObject(const IObjectType* innerObject) = 0;
  
  /**
   * Finds an inner object with the given short name that is defined within this object
   */
  virtual const IObjectType* getInnerObject(std::string shortName) const = 0;
  
  /**
   * Returns an i8* constant pointer to the name of the collable object
   */
  static llvm::Constant* getObjectNamePointer(const IObjectType* object,
                                              IRGenerationContext& context);

  /**
   * Returns the reference count value for the given object
   */
  static llvm::Value* getReferenceCountForObject(IRGenerationContext& context, llvm::Value* object);
  
  /**
   * Checks type of the object value, casts it to int64* type and returns it
   */
  static llvm::Value* getReferenceCounterPointer(IRGenerationContext& context, llvm::Value* object);
  
  /**
   * Check that the private object is accessable
   */
  static bool checkAccess(const IObjectType* from, const IObjectType* to);
  
};
  
} /* namespace wisey */

#endif /* IObjectType_h */
