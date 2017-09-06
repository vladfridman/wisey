//
//  StringConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "llvm/IR/Constants.h"

#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringConstant.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* StringConstant::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* StringConstant::generateIR(IRGenerationContext& context) const {
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

string StringConstant::unescape(const string& input) {
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

const IType* StringConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::VOID_TYPE;
}

void StringConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a string constant, it is not a heap pointer");
  exit(1);
}

void StringConstant::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to a string constant expression");
  exit(1);
}

bool StringConstant::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
