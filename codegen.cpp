#include "node.hpp"
#include "codegen.hpp"
#include "y.tab.h"

using namespace std;

namespace yazyk {

/* Compile the AST into a module */
void IRGenerationContext::generateIR(Block& root) {
  cout << "Generating code...\n";
  
  owner = make_unique<Module>("test", TheContext);
  module = owner.get();
  
  root.generateIR(*this);
  
  cout << "Code is generated.\n";
  verifyModule(*module);
  cout << "Code is verified.\n";
  
  legacy::PassManager passManager;

  // print out assembly code
  // passManager.add(createPrintModulePass(outs()));

  // Optimization: Constant Propagation transform
  // passManager.add(createConstantPropagationPass());
  
  // Optimization: Dead Instruction Elimination transform
  // passManager.add(createDeadInstEliminationPass());

  // print out assembly code
  passManager.add(createPrintModulePass(outs()));

  passManager.run(*module);
  
  cout << "PM has been run.\n";
}

/* Executes the AST by running the main function */
GenericValue IRGenerationContext::runCode() {
  cout << "Running code...\n";
  ExecutionEngine *executionEngine = EngineBuilder(move(owner)).create();
  vector<GenericValue> noargs;
  if (mainFunction == NULL) {
    cerr << "Function main() is not defined. Exiting." << endl;
    delete executionEngine;
    exit(1);
  }
  GenericValue result = executionEngine->runFunction(mainFunction, noargs);
  cout << "Code was run.\n";
  outs() << "Result: " << result.IntVal << "\n";
  delete executionEngine;
  
  return result;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const TypeSpecifier& type) {
  if (type.type == PRIMITIVE_TYPE_INT) {
    return Type::getInt32Ty(TheContext);
  } else if (type.type == PRIMITIVE_TYPE_LONG) {
    return Type::getInt64Ty(TheContext);
  } else if (type.type == PRIMITIVE_TYPE_FLOAT) {
    return Type::getFloatTy(TheContext);
  } else if (type.type == PRIMITIVE_TYPE_DOUBLE) {
    return Type::getDoubleTy(TheContext);
  }

  return Type::getVoidTy(TheContext);
}

/* -- Code Generation -- */

Value* Integer::generateIR(IRGenerationContext& context) {
  cout << "Creating integer: " << value << endl;
  return ConstantInt::get(Type::getInt32Ty(TheContext), value, true);
}

Value* Double::generateIR(IRGenerationContext& context) {
  cout << "Creating double: " << value << endl;
  return ConstantFP::get(Type::getDoubleTy(TheContext), value);
}

Value* Identifier::generateIR(IRGenerationContext& context) {
  cout << "Creating identifier reference: " << name << endl;
  if (context.locals().find(name) == context.locals().end()) {
    cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[name], "", context.currentBlock());
}

Value * TypeSpecifier::generateIR(IRGenerationContext &context) {
  return NULL;
}

Value* MethodCall::generateIR(IRGenerationContext& context) {
  Function *function = context.getModule()->getFunction(id.name.c_str());
  if (function == NULL) {
    cerr << "no such function " << id.name << endl;
  }
  vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    args.push_back((**it).generateIR(context));
  }
  CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
  cout << "Creating method call: " << id.name << endl;
  return call;
}

Value* AddditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) {
  cout << "Creating binary operation " << operation << endl;
  Instruction::BinaryOps instruction;
  string name;
  switch (operation) {
    case '+': name = "add"; instruction = Instruction::Add; break;
    case '-': name = "sub"; instruction = Instruction::Sub; break;
    case '*': name = "mul"; instruction = Instruction::Mul; break;
    case '/': name = "div"; instruction = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value * lhsValue = lhs.generateIR(context);
  Value * rhsValue = rhs.generateIR(context);

  return llvm::BinaryOperator::Create(instruction,
                                      lhsValue,
                                      rhsValue,
                                      name,
                                      context.currentBlock());
}
  
Value* RelationalExpression::generateIR(IRGenerationContext& context) {
  cout << "Creating relational expression " << operation << endl;
  ICmpInst::Predicate predicate;
  switch (operation) {
    case RELATIONAL_OPERATION_LT : predicate = ICmpInst::ICMP_SLT; break;
    case RELATIONAL_OPERATION_GT : predicate = ICmpInst::ICMP_SGT; break;
    case RELATIONAL_OPERATION_LE : predicate = ICmpInst::ICMP_SLE; break;
    case RELATIONAL_OPERATION_GE : predicate = ICmpInst::ICMP_SGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
    default: return NULL;
  }
  
  Value * lhsValue = lhs.generateIR(context);
  Value * rhsValue = rhs.generateIR(context);
  
  return new ICmpInst(*context.currentBlock(),
                      predicate,
                      lhsValue,
                      rhsValue,
                      "cmp");
}

Value *LogicalAndExpression::generateIR(IRGenerationContext& context) {
  cout << "Creating logical AND expression " << endl;
  
  Value * lhsValue = lhs.generateIR(context);
  BasicBlock * entryBlock = context.currentBlock();
  
  Function * function = context.currentBlock()->getParent();
  
  BasicBlock *bblockRhs = BasicBlock::Create(TheContext, "land.rhs", function);
  BasicBlock *bblockEnd = BasicBlock::Create(TheContext, "land.end", function);
  BranchInst::Create(bblockRhs, bblockEnd, lhsValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value * rhsValue = rhs.generateIR(context);
  BasicBlock * lastRhsBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  PHINode * phiNode = PHINode::Create(Type::getInt1Ty(TheContext), 0, "", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getFalse(TheContext), entryBlock);
  phiNode->addIncoming(rhsValue, lastRhsBlock);
  
  return phiNode;
}
  
Value *LogicalOrExpression::generateIR(IRGenerationContext& context) {
  cout << "Creating logical OR expression " << endl;
  
  Value * lhsValue = lhs.generateIR(context);
  BasicBlock * entryBlock = context.currentBlock();
  
  Function * function = context.currentBlock()->getParent();
  
  BasicBlock *bblockRhs = BasicBlock::Create(TheContext, "lor.rhs", function);
  BasicBlock *bblockEnd = BasicBlock::Create(TheContext, "lor.end", function);
  BranchInst::Create(bblockEnd, bblockRhs, lhsValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value * rhsValue = rhs.generateIR(context);
  BasicBlock * lastRhsBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  PHINode * phiNode = PHINode::Create(Type::getInt1Ty(TheContext), 0, "", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getTrue(TheContext), entryBlock);
  phiNode->addIncoming(rhsValue, lastRhsBlock);
  
  return phiNode;
}
    
Value* Assignment::generateIR(IRGenerationContext& context) {
  cout << "Creating assignment for " << lhs.name << endl;
  if (context.locals().find(lhs.name) == context.locals().end()) {
    cerr << "undeclared variable " << lhs.name << endl;
    return NULL;
  }
  return new StoreInst(rhs.generateIR(context), context.locals()[lhs.name], context.currentBlock());
}

Value* Block::generateIR(IRGenerationContext& context) {
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    IStatement *statement = *it;
    cout << "Generating block code for " << typeid(*statement).name() << endl;
    last = statement->generateIR(context);
  }
  cout << "Creating block" << endl;
  return last;
}

Value* ExpressionStatement::generateIR(IRGenerationContext& context) {
  cout << "Generating expression statement code for " << typeid(expression).name() << endl;
  return expression.generateIR(context);
}

Value* VariableDeclaration::generateIR(IRGenerationContext& context) {
  cout << "Creating variable declaration " << type.type << " " << id.name << endl;
  AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
  context.locals()[id.name] = alloc;
  if (assignmentExpr != NULL) {
    Assignment assn(id, *assignmentExpr);
    assn.generateIR(context);
  }
  return alloc;
}


Value* ReturnStatement::generateIR(IRGenerationContext& context) {
  cout << "Generatring return statement" << endl;
  
  Value* returnValue = expression.generateIR(context);
  Type* valueType = returnValue->getType();
  Function *parentFunction = context.currentBlock()->getParent();

  if (parentFunction == NULL) {
    cerr << "No corresponding method found for RETURN" << endl;
    exit(1);
  }
  
  Type * returnType = parentFunction->getReturnType();
  
  if (returnType != valueType &&
      !CastInst::isCastable(valueType, returnType)) {
    cerr << "Can not cast return value to function type" << endl;
    exit(1);
  }
  
  if (returnType != valueType) {
    returnValue = CastInst::CreateZExtOrBitCast(returnValue,
                                                returnType,
                                                "conv",
                                                context.currentBlock());
  }
  
  ReturnInst* result = ReturnInst::Create(TheContext,
                                          returnValue,
                                          context.currentBlock());
  return result;
}

Value* FunctionDeclaration::generateIR(IRGenerationContext& context) {
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    argTypes.push_back(typeOf((**it).type));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType *ftype = FunctionType::get(typeOf(type), argTypesArray, false);
  Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.getModule());
  if (strcmp(id.name.c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  Function::arg_iterator args = function->arg_begin();
  for (it = arguments.begin(); it != arguments.end(); it++) {
    Argument *arg = &*args;
    arg->setName((**it).id.name);
  }
  BasicBlock *bblock = BasicBlock::Create(TheContext, "entry", function, 0);
  
  context.pushBlock(bblock);

  args = function->arg_begin();
  for (it = arguments.begin(); it != arguments.end(); it++) {
    Value *value = &*args;
    string newName = (**it).id.name + ".param";
    AllocaInst *alloc = new AllocaInst(typeOf((**it).type), newName, bblock);
    value = new StoreInst(value, alloc, bblock);
    context.locals()[(**it).id.name] = alloc;
  }

  block.generateIR(context);
  
  context.popBlock();
  cout << "Creating function: " << id.name << endl;
  return function;
}

} // namespace yazyk