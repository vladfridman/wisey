//
//  FieldInjected.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldInjected_h
#define FieldInjected_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IField.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents an injected field in a controller
 */
class FieldInjected : public IField {
  const IType* mType;
  std::string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldInjected(const IType* type, std::string name, unsigned long index, ExpressionList arguments) :
  mType(type), mName(name), mIndex(index), mArguments(arguments) { }
  
  ~FieldInjected();
  
  const IType* getType() const override;
  
  std::string getName() const override;
  
  unsigned long getIndex() const override;
  
  ExpressionList getArguments() const override;
  
  bool isAssignable() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* FieldInjected_h */
