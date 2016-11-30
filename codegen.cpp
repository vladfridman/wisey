#include "node.hpp"
#include "codegen.hpp"
#include "y.tab.h"

using namespace std;

/* Compile the AST into a module */
void IRGenerationContext::generateIR(YazBlock& root) {
  cout << "Generating code...\n";
  
  owner = make_unique<Module>("test", TheContext);
  module = owner.get();
  
  root.generateIR(*this);
  
  /**
   * Print the bytecode in a human-readable format
   * to see if our program compiled properly
   */
  cout << "Code is generated.\n";
  verifyModule(*module);
  cout << "Code is verified.\n";
  
  legacy::PassManager pm;
  pm.add(createPrintModulePass(outs()));
  pm.run(*module);
  
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
static Type *typeOf(const YazTypeSpecifier& type) {
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

Value* YazInteger::generateIR(IRGenerationContext& context) {
  cout << "Creating integer: " << value << endl;
  return ConstantInt::get(Type::getInt32Ty(TheContext), value, true);
}

Value* YazDouble::generateIR(IRGenerationContext& context) {
  cout << "Creating double: " << value << endl;
  return ConstantFP::get(Type::getDoubleTy(TheContext), value);
}

Value* YazIdentifier::generateIR(IRGenerationContext& context) {
  cout << "Creating identifier reference: " << name << endl;
  if (context.locals().find(name) == context.locals().end()) {
    cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[name], "", context.currentBlock());
}

Value * YazTypeSpecifier::generateIR(IRGenerationContext &context) {
  return NULL;
}

Value* YazMethodCall::generateIR(IRGenerationContext& context) {
  Function *function = context.getModule()->getFunction(id.name.c_str());
  if (function == NULL) {
    cerr << "no such function " << id.name << endl;
  }
  vector<Value*> args;
  YazExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    args.push_back((**it).generateIR(context));
  }
  CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
  cout << "Creating method call: " << id.name << endl;
  return call;
}

Value* YazBinaryOperator::generateIR(IRGenerationContext& context) {
  cout << "Creating binary operation " << op << endl;
  Instruction::BinaryOps instr;
  switch (op) {
    case '+': instr = Instruction::Add; break;
    case '-': instr = Instruction::Sub; break;
    case '*': instr = Instruction::Mul; break;
    case '/': instr = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value * lhsValue = lhs.generateIR(context);
  Value * rhsValue = rhs.generateIR(context);

  return BinaryOperator::Create(instr,
                                lhsValue,
                                rhsValue,
                                "",
                                context.currentBlock());
}

Value* YazAssignment::generateIR(IRGenerationContext& context) {
  cout << "Creating assignment for " << lhs.name << endl;
  if (context.locals().find(lhs.name) == context.locals().end()) {
    cerr << "undeclared variable " << lhs.name << endl;
    return NULL;
  }
  return new StoreInst(rhs.generateIR(context), context.locals()[lhs.name], context.currentBlock());
}

Value* YazBlock::generateIR(IRGenerationContext& context) {
  YazStatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    IYazStatement *statement = *it;
    cout << "Generating block code for " << typeid(*statement).name() << endl;
    last = statement->generateIR(context);
  }
  cout << "Creating block" << endl;
  return last;
}

Value* YazExpressionStatement::generateIR(IRGenerationContext& context) {
  cout << "Generating expression statement code for " << typeid(expression).name() << endl;
  return expression.generateIR(context);
}

Value* YazVariableDeclaration::generateIR(IRGenerationContext& context) {
  cout << "Creating variable declaration " << type.type << " " << id.name << endl;
  AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
  context.locals()[id.name] = alloc;
  if (assignmentExpr != NULL) {
    YazAssignment assn(id, *assignmentExpr);
    assn.generateIR(context);
  }
  return alloc;
}


Value* YazReturnStatement::generateIR(IRGenerationContext& context) {
  cout << "Generatring return statement" << endl;

  Value * result = ReturnInst::Create(TheContext,
                                      expression.generateIR(context),
                                      context.currentBlock());
  return result;
}

Value* YazFunctionDeclaration::generateIR(IRGenerationContext& context) {
  vector<Type*> argTypes;
  YazVariableList::const_iterator it;
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

