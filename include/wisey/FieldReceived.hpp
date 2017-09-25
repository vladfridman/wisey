//
//  FieldReceived.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldReceived_h
#define FieldReceived_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IField.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents a received field in a controller
 */
class FieldReceived : public IField {
  const IType* mType;
  std::string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldReceived(const IType* type,
                std::string name,
                unsigned long index,
                ExpressionList arguments) :
  mType(type), mName(name), mIndex(index), mArguments(arguments) { }
  
  ~FieldReceived();
  
  const IType* getType() const override;
  
  std::string getName() const override;
  
  unsigned long getIndex() const override;
  
  ExpressionList getArguments() const override;
  
  bool isAssignable() const override;
  
  void extractHeader(std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* FieldReceived_h */
