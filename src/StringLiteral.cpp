//
//  StringLiteral.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "llvm/IR/Constants.h"

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringLiteral.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* wisey::StringLiteral::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* wisey::StringLiteral::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, mValue);
  GlobalVariable* globalVariableString =
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     stringConstant,
                     ".str");
  
  Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(llvmContext));
  Constant* indices[] = {zero, zero};

  return ConstantExpr::getGetElementPtr(NULL, globalVariableString, indices, true);
}

string wisey::StringLiteral::unescape(const string& input) {
  string result;
  string::const_iterator iterator = input.begin();
  while (iterator != input.end())
  {
    char currentChar = *iterator++;
    if (currentChar == '\\' && iterator != input.end())
    {
      switch (*iterator++) {
        case '\\': currentChar = '\\'; break;
        case 'n': currentChar = '\n'; break;
        case 't': currentChar = '\t'; break;
          // all other escapes
        default:
          // invalid escape sequence - skip it.
          continue;
      }
    }
    result += currentChar;
  }
  
  return result;
}

const IType* wisey::StringLiteral::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::STRING_TYPE;
}

void wisey::StringLiteral::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a string literal, it is not a heap pointer");
  exit(1);
}

void wisey::StringLiteral::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to a string literal expression");
  exit(1);
}

bool wisey::StringLiteral::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

void wisey::StringLiteral::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "\"" << mValue << "\"";
}
