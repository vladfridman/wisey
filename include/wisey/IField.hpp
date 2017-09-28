//
//  IField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IField_h
#define IField_h

#include "wisey/FieldKind.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/IType.hpp"

namespace wisey {

/**
 * Interface representing a concrete object's field
 */
class IField : public IPrintable {
    
public:
  
  virtual ~IField() { };
  
  /**
   * Returns field type
   */
  virtual const IType* getType() const = 0;
  
  /**
   * Returns field name
   */
  virtual std::string getName() const = 0;
  
  /**
   * Returns field index in the struct of the object
   */
  virtual unsigned long getIndex() const = 0;
  
  /**
   * Returns field initialization arguments
   */
  virtual ExpressionList getArguments() const = 0;
  
  /**
   * Tells whether this field is assignable
   */
  virtual bool isAssignable() const = 0;
  
  /**
   * Returns field kind: fixed, state, injected or received
   */
  virtual FieldKind getFieldKind() const = 0;
  
};

} /* namespace wisey */

#endif /* IField_h */
