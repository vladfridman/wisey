//
//  Constant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Constant_h
#define Constant_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/IPrintable.hpp"

namespace wisey {
  
/**
 * Represents a constant defined within an object
 */
class Constant : public IPrintable, public IObjectElement {
  const AccessLevel mAccessLevel;
  const IType* mType;
  std::string mName;
  IExpression* mExpression;

public:
  
  Constant(const AccessLevel accessLevel,
           const IType* type,
           std::string name,
           IExpression* expression);
  
  ~Constant();
  
  ObjectElementType getObjectElementType() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* Constant_h */
