//
//  IField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IField_h
#define IField_h

#include "IObjectElement.hpp"
#include "IPrintable.hpp"
#include "IType.hpp"

namespace wisey {
  
  class IConcreteObjectType;
  
  /**
   * Represents a field in a concrete object
   */
  class IField : public IObjectElement, public IPrintable {
    
  public:
    
    /**
     * Returns field name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Returns field type
     */
    virtual const IType* getType() const = 0;
    
    /**
     * Returns field value
     */
    virtual llvm::Value* getValue(IRGenerationContext& context,
                                  const IConcreteObjectType* object,
                                  llvm::Value* fieldPointer,
                                  int line) const = 0;
    
    /**
     * Returns the line where the field is defined
     */
    virtual int getLine() const = 0;
    
    /**
     * Tells whether this field is assignable
     */
    virtual bool isAssignable(const IConcreteObjectType* object) const = 0;
    
    /**
     * Returns field kind in a form of a string
     */
    virtual std::string getFieldKind() const = 0;
    
    /**
     * Methods for determining field kind
     */
    virtual bool isInjected() const = 0;
    
    virtual bool isReceived() const = 0;
    
    virtual bool isState() const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IField_h */

