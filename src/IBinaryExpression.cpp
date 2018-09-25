//
//  IBinaryExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "IBinaryExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void IBinaryExpression::printToStream(IRGenerationContext& context,
                                      iostream& stream,
                                      const IBinaryExpression* expression) {
  expression->getLeft()->printToStream(context, stream);
  stream << " " << expression->getOperation() << " ";
  expression->getRight()->printToStream(context, stream);
}
