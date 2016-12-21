#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/node.hpp"
#include "yazyk/log.hpp"
#include "yazyk/TypeIdentifier.hpp"
#include "y.tab.h"

using namespace llvm;
using namespace std;

namespace yazyk {

/* -- Code Generation -- */

Value* Char::generateIR(IRGenerationContext& context) {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), value);
}
  
Value* Integer::generateIR(IRGenerationContext& context) {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), value, true);
}

Value* Long::generateIR(IRGenerationContext& context) {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), value, true);
}

Value* Float::generateIR(IRGenerationContext& context) {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), value);
}
  
Value* Double::generateIR(IRGenerationContext& context) {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), value);
}

Value* String::generateIR(IRGenerationContext& context) {
  Constant* strConstant = ConstantDataArray::getString(context.getLLVMContext(), value);
  GlobalVariable* globalVariableString =
    new GlobalVariable(*context.getModule(),
                       strConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       strConstant,
                       ".str");

  Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(context.getLLVMContext()));
  Constant* indices[] = {zero, zero};
  Constant* strVal = ConstantExpr::getGetElementPtr(NULL,
                                                    globalVariableString,
                                                    indices,
                                                    true);

  return strVal;
}
  
string String::unescape(const string& input) {
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
  
Function* MethodCall::declarePrintf(IRGenerationContext& context) {
  FunctionType *printf_type = TypeBuilder<int(char *, ...), false>::get(context.getLLVMContext());
    
  Function *function = cast<Function>(
    context.getModule()->getOrInsertFunction("printf",
                                             printf_type,
                                             AttributeSet().addAttribute(context.getLLVMContext(), 1U, Attribute::NoAlias)));
  
  return function;
}
  
Value* MethodCall::generateIR(IRGenerationContext& context) {
  Function *function = context.getModule()->getFunction(id.getName().c_str());
  if (function == NULL && id.getName().compare("printf") != 0) {
    cerr << "no such function " << id.getName() << endl;
  }
  if (function == NULL) {
    function = declarePrintf(context);
  }
  vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    args.push_back((**it).generateIR(context));
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";
  CallInst *call = CallInst::Create(function, args, resultName, context.currentBlock());
  return call;
}

Value* Assignment::generateIR(IRGenerationContext& context) {
  if (context.locals().find(lhs.getName()) == context.locals().end()) {
    Log::e("undeclared variable " + lhs.getName());
    return NULL;
  }
  return new StoreInst(rhs.generateIR(context), context.locals()[lhs.getName()], context.currentBlock());
}

} /* namespace yazyk */
