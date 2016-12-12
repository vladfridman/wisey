#include "yazyk/node.hpp"
#include "yazyk/codegen.hpp"
#include "yazyk/log.hpp"
#include "y.tab.h"

using namespace llvm;
using namespace std;

namespace yazyk {

/* Compile the AST into a module */
void IRGenerationContext::generateIR(Block& root) {
  owner = make_unique<Module>("test", getLLVMContext());
  module = owner.get();

  root.generateIR(*this);
  
  verifyModule(*module);
  
  legacy::PassManager passManager;

  // Optimization: Constant Propagation transform
  // passManager.add(createConstantPropagationPass());
  // Optimization: Dead Instruction Elimination transform
  // passManager.add(createDeadInstEliminationPass());

  // print out assembly code
  if (Log::isDebugLevel()) {
    passManager.add(createPrintModulePass(outs()));
  }

  passManager.run(*module);
}

/* Executes the AST by running the main function */
GenericValue IRGenerationContext::runCode() {
  ExecutionEngine *executionEngine = EngineBuilder(move(owner)).create();
  vector<GenericValue> noargs;
  if (mainFunction == NULL) {
    Log::e("Function main() is not defined. Exiting.");
    delete executionEngine;
    exit(1);
  }
  Log::i("Running program:");
  GenericValue result = executionEngine->runFunction(mainFunction, noargs);
  Log::i("Result: " + result.IntVal.toString(10, true));
  delete executionEngine;
  
  return result;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(LLVMContext &llvmContext, const TypeSpecifier& type) {
  if (type.type == PRIMITIVE_TYPE_INT) {
    return Type::getInt32Ty(llvmContext);
  } else if (type.type == PRIMITIVE_TYPE_LONG) {
    return Type::getInt64Ty(llvmContext);
  } else if (type.type == PRIMITIVE_TYPE_FLOAT) {
    return Type::getFloatTy(llvmContext);
  } else if (type.type == PRIMITIVE_TYPE_DOUBLE) {
    return Type::getDoubleTy(llvmContext);
  } else if (type.type == PRIMITIVE_TYPE_VOID) {
    return Type::getVoidTy(llvmContext);
  }

  Log::e("Unknown type " + to_string(type.type) + ". Replacing with void");
  return Type::getVoidTy(llvmContext);
}

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

Value* Identifier::generateIR(IRGenerationContext& context) {
  if (context.locals().find(name) == context.locals().end()) {
    cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[name], "", context.currentBlock());
}

Value * TypeSpecifier::generateIR(IRGenerationContext &context) {
  return NULL;
}

Function* MethodCall::declarePrintf(IRGenerationContext& context) {
  FunctionType *printf_type = TypeBuilder<int(char *, ...), false>::get(context.getLLVMContext());
  
  Function *func = cast<Function>(
    context.getModule()->getOrInsertFunction("printf",
      printf_type,
      AttributeSet().addAttribute(context.getLLVMContext(), 1U, Attribute::NoAlias)));
  return func;
}
  
Value* MethodCall::generateIR(IRGenerationContext& context) {
  Function *function = context.getModule()->getFunction(id.name.c_str());
  if (function == NULL && id.name.compare("printf") != 0) {
    cerr << "no such function " << id.name << endl;
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

Value* IncrementExpression::generateIR(IRGenerationContext& context) {
  Value* originalValue = identifier.generateIR(context);
  Value *increment = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()),
                                      incrementBy,
                                      true);

  Value *incrementResult = llvm::BinaryOperator::Create(Instruction::Add,
                                                        originalValue,
                                                        increment,
                                                        variableName,
                                                        context.currentBlock());
  new StoreInst(incrementResult, context.locals()[identifier.name], context.currentBlock());
  return isPrefix ? incrementResult : originalValue;
}
  
Value *LogicalAndExpression::generateIR(IRGenerationContext& context) {
  Value * lhsValue = lhs.generateIR(context);
  BasicBlock * entryBlock = context.currentBlock();
  
  Function * function = context.currentBlock()->getParent();
  
  BasicBlock *bblockRhs = BasicBlock::Create(context.getLLVMContext(), "land.rhs", function);
  BasicBlock *bblockEnd = BasicBlock::Create(context.getLLVMContext(), "land.end", function);
  BranchInst::Create(bblockRhs, bblockEnd, lhsValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value * rhsValue = rhs.generateIR(context);
  BasicBlock * lastRhsBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  Type * type = Type::getInt1Ty(context.getLLVMContext());
  PHINode * phiNode = PHINode::Create(type, 0, "", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getFalse(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rhsValue, lastRhsBlock);
  
  return phiNode;
}
  
Value *LogicalOrExpression::generateIR(IRGenerationContext& context) {
  Value * lhsValue = lhs.generateIR(context);
  BasicBlock * entryBlock = context.currentBlock();
  
  Function * function = context.currentBlock()->getParent();
  
  BasicBlock *bblockRhs = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock *bblockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  BranchInst::Create(bblockEnd, bblockRhs, lhsValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value * rhsValue = rhs.generateIR(context);
  BasicBlock * lastRhsBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  Type * type = Type::getInt1Ty(context.getLLVMContext());
  PHINode * phiNode = PHINode::Create(type, 0, "", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getTrue(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rhsValue, lastRhsBlock);
  
  return phiNode;
}

Value *ConditionalExpression::generateIR(IRGenerationContext& context) {
  Value * conditionValue = conditionExpression.generateIR(context);
  
  Function * function = context.currentBlock()->getParent();
  
  BasicBlock *bblockCondTrue = BasicBlock::Create(context.getLLVMContext(), "cond.true", function);
  BasicBlock *bblockCondFalse =
    BasicBlock::Create(context.getLLVMContext(), "cond.false", function);
  BasicBlock *bblockCondEnd = BasicBlock::Create(context.getLLVMContext(), "cond.end", function);
  BranchInst::Create(bblockCondTrue, bblockCondFalse, conditionValue, context.currentBlock());
  
  context.replaceBlock(bblockCondTrue);
  Value * condTrueValue = conditionTrueExpression.generateIR(context);
  Type * condTrueResultType = condTrueValue->getType();
  BasicBlock * lastBlock = context.currentBlock();
  BranchInst::Create(bblockCondEnd, context.currentBlock());

  context.replaceBlock(bblockCondFalse);
  Value * condFalseValue = conditionFalseExpression.generateIR(context);
  Type * condFalseResultType = condTrueValue->getType();
  lastBlock = context.currentBlock();
  BranchInst::Create(bblockCondEnd, context.currentBlock());

  if (condTrueResultType != condFalseResultType) {
    Log::e("Results of different type in a conditional expresion!");
  }
  
  context.replaceBlock(bblockCondEnd);
  PHINode * phiNode = PHINode::Create(condTrueResultType,
                                      0,
                                      "cond",
                                      context.currentBlock());
  phiNode->addIncoming(condTrueValue, bblockCondTrue);
  phiNode->addIncoming(condFalseValue, bblockCondFalse);
  
  return phiNode;
}
    
Value* Assignment::generateIR(IRGenerationContext& context) {
  if (context.locals().find(lhs.name) == context.locals().end()) {
    Log::e("undeclared variable " + lhs.name);
    return NULL;
  }
  return new StoreInst(rhs.generateIR(context), context.locals()[lhs.name], context.currentBlock());
}

Value* Block::generateIR(IRGenerationContext& context) {
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    IStatement *statement = *it;
    last = statement->generateIR(context);
  }
  return last;
}

Value* ExpressionStatement::generateIR(IRGenerationContext& context) {
  return expression.generateIR(context);
}

Value* VariableDeclaration::generateIR(IRGenerationContext& context) {
  AllocaInst *alloc = new AllocaInst(typeOf(context.getLLVMContext(), type),
                                     id.name.c_str(),
                                     context.currentBlock());
  context.locals()[id.name] = alloc;
  if (assignmentExpr != NULL) {
    Assignment assn(id, *assignmentExpr);
    assn.generateIR(context);
  }
  return alloc;
}


Value* ReturnStatement::generateIR(IRGenerationContext& context) {
  Value* returnValue = expression.generateIR(context);
  Type* valueType = returnValue->getType();
  Function *parentFunction = context.currentBlock()->getParent();

  if (parentFunction == NULL) {
    Log::e("No corresponding method found for RETURN");
    exit(1);
  }
  
  Type * returnType = parentFunction->getReturnType();
  
  if (returnType != valueType &&
      !CastInst::isCastable(valueType, returnType)) {
    Log::e("Can not cast return value to function type");
    exit(1);
  }
  
  if (returnType != valueType) {
    returnValue = CastInst::CreateZExtOrBitCast(returnValue,
                                                returnType,
                                                "conv",
                                                context.currentBlock());
  }
  
  ReturnInst* result = ReturnInst::Create(context.getLLVMContext(),
                                          returnValue,
                                          context.currentBlock());
  return result;
}

Value* ReturnVoidStatement::generateIR(IRGenerationContext& context) {
  return ReturnInst::Create(context.getLLVMContext(), NULL, context.currentBlock());
}
  
Value* FunctionDeclaration::generateIR(IRGenerationContext& context) {
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    argTypes.push_back(typeOf(context.getLLVMContext(), (**it).type));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType *ftype = FunctionType::get(typeOf(context.getLLVMContext(), type),
                                          argTypesArray,
                                          false);
  Function *function = Function::Create(ftype,
                                        GlobalValue::InternalLinkage,
                                        id.name.c_str(),
                                        context.getModule());
  if (strcmp(id.name.c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  Function::arg_iterator args = function->arg_begin();
  for (it = arguments.begin(); it != arguments.end(); it++) {
    Argument *arg = &*args;
    arg->setName((**it).id.name);
  }
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  
  context.pushBlock(bblock);

  args = function->arg_begin();
  for (it = arguments.begin(); it != arguments.end(); it++) {
    Value *value = &*args;
    string newName = (**it).id.name + ".param";
    AllocaInst *alloc = new AllocaInst(typeOf(context.getLLVMContext(), (**it).type),
                                       newName,
                                       bblock);
    value = new StoreInst(value, alloc, bblock);
    context.locals()[(**it).id.name] = alloc;
  }

  block.generateIR(context);
  
  context.popBlock();
  return function;
}

} // namespace yazyk