//
//  IExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/UndefinedType.hpp"

using namespace std;
using namespace wisey;

void IExpression::checkForUndefined(IRGenerationContext& context,
                                    const IExpression* expression) {
  const IType* expressionType = expression->getType(context);
  if (expressionType != UndefinedType::UNDEFINED && !expressionType->isPackage()) {
    return;
  }

  stringstream stringStream;
  expression->printToStream(context, stringStream);
  context.reportError(expression->getLine(), "Undefined expression '" + stringStream.str() + "'");
  throw 1;
}
