//
//  Field.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Field_h
#define Field_h

#include <llvm/IR/Instructions.h>

#include "wisey/FieldKind.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/IType.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
/**
 * Represents a field in a concrete object
 */
class Field : public IPrintable, public IObjectElement {
  const FieldKind mFieldKind;
  const IType* mType;
  const IType* mInjectedType;
  std::string mName;
  InjectionArgumentList mInjectionArgumentList;
  
public:
  
  Field(FieldKind fieldKind,
        const IType* type,
        const IType* injectedType,
        std::string name,
        InjectionArgumentList injectionArgumentList);
  
  ~Field();
  
  /**
   * Returns field kind: fixed, state, injected or received
   */
  FieldKind getFieldKind() const;
  
  /**
   * Returns field type
   */
  const IType* getType() const;
  
  /**
   * Returns type that should be injected and assigned to the field when injecting an object
   */
  const IType* getInjectedType() const;
  
  /**
   * Returns field name
   */
  std::string getName() const;
  
  /**
   * Returns field index in the struct of the object
   */
  unsigned long getIndex() const;
  
  /**
   * Returns field injection arguments for injected fields
   */
  InjectionArgumentList getInjectionArguments() const;
  
  /**
   * Tells whether this field is assignable
   */
  bool isAssignable() const;
  
  ObjectElementType getObjectElementType() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* Field_h */
