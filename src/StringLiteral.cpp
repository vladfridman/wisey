//
//  StringLiteral.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "llvm/IR/Constants.h"

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"
#include "StringLiteral.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

wisey::StringLiteral::StringLiteral(string input, int line) : mValue(unescape(input)), mLine(line) {
}

wisey::StringLiteral::~StringLiteral() {
}

int wisey::StringLiteral::getLine() const {
  return mLine;
}

llvm::Constant* wisey::StringLiteral::generateIR(IRGenerationContext& context,
                                                 const IType* assignToType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, mValue);
  GlobalVariable* globalVariableString =
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     stringConstant,
                     ".str");
  
  llvm::Constant* zero = llvm::Constant::getNullValue(IntegerType::getInt32Ty(llvmContext));
  llvm::Constant* indices[] = {zero, zero};

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
  return PrimitiveTypes::STRING;
}

bool wisey::StringLiteral::isConstant() const {
  return true;
}

bool wisey::StringLiteral::isAssignable() const {
  return false;
}

void wisey::StringLiteral::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "\"" << mValue << "\"";
}
