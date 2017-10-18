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
#include "wisey/IPrintable.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents a field in a concrete object
 */
class Field : public IPrintable {
  const FieldKind mFieldKind;
  const IType* mType;
  std::string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  Field(FieldKind fieldKind,
        const IType* type,
        std::string name,
        unsigned long index,
        ExpressionList arguments);
  
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
   * Returns field name
   */
  std::string getName() const;
  
  /**
   * Returns field index in the struct of the object
   */
  unsigned long getIndex() const;
  
  /**
   * Returns field initialization arguments
   */
  ExpressionList getArguments() const;
  
  /**
   * Tells whether this field is assignable
   */
  bool isAssignable() const;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* Field_h */
