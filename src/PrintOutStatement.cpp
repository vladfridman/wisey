//
//  PrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrintOutStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintOutStatement::~PrintOutStatement() {
  delete mStringLiteral;
}

Value* PrintOutStatement::generateIR(IRGenerationContext& context) const {
  Value* stringValue = mStringLiteral->generateIR(context);

  Function* printf = IntrinsicFunctions::getPrintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(stringValue);
  return IRWriter::createCallInst(context, printf, arguments, "");
}
