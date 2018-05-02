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

#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IReferenceType.hpp"
#include "wisey/ImportProfile.hpp"

namespace wisey {
  
  class Constant;
  class IMethodDescriptor;
  class LLVMFunction;
  
  /**
   * Represents an IType that is either model, controller or interface
   */
  class IObjectType : public IReferenceType {
    
  public:
    
    /**
     * Name of the variable referring to the current object
     */
    static std::string THIS;
    
    /**
     * Tells whether the object is publicly accessable
     */
    virtual bool isPublic() const = 0;
    
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
    virtual Constant* findConstant(IRGenerationContext& context,
                                   std::string constantName,
                                   int line) const = 0;
    
    /**
     * Finds a llvm function with a given name.
     */
    virtual LLVMFunction* findLLVMFunction(std::string functionName) const = 0;

    /**
     * Returns name of the global variable containing name of this Callable Object in string format
     */
    virtual std::string getObjectNameGlobalVariableName() const = 0;
    
    /**
     * Returns the owner type for this object
     */
    virtual const IObjectOwnerType* getOwner() const override = 0;
    
    /**
     * Tells whether this object is externally implemented in a shared library
     */
    virtual bool isExternal() const = 0;
    
    /**
     * Override method from IType because ObjectOwner llvm type is always a PointerType
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const override = 0;
    
    /**
     * Returns pointer reference counter value for this object
     */
    virtual llvm::Value* getReferenceCount(IRGenerationContext& context,
                                           llvm::Value* object) const = 0;
    
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
     * Returns all inner objects
     */
    virtual std::map<std::string, const IObjectType*> getInnerObjects() const = 0;
    
    /**
     * Marks this object as inner of some other object
     */
    virtual void markAsInner() = 0;
    
    /**
     * Tells whether this object is inner
     */
    virtual bool isInner() const = 0;
    
    /**
     * Returns a function that is used to adjust the number of reference for this object type
     */
    virtual llvm::Function* getReferenceAdjustmentFunction(IRGenerationContext& context) const = 0;
    
    /**
     * Returns the line number where this object is defined
     */
    virtual int getLine() const = 0;

    /**
     * Returns an i8* constant pointer to the name of the collable object
     */
    static llvm::Constant* getObjectNamePointer(const IObjectType* object,
                                                IRGenerationContext& context);
    
    /**
     * Returns the reference count value for the given object
     */
    static llvm::Value* getReferenceCountForObject(IRGenerationContext& context,
                                                   llvm::Value* object);
    
    /**
     * Check that the private object is accessable
     */
    static bool checkAccess(const IObjectType* from, const IObjectType* to);

  };
  
} /* namespace wisey */

#endif /* IObjectType_h */

