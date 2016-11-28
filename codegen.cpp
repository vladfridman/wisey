#include "node.hpp"
#include "codegen.hpp"
#include "y.tab.h"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root)
{
  std::cout << "Generating code...\n";
  
  owner = make_unique<Module>("test", TheContext);
  module = owner.get();
  
  /* Create the top level interpreter function to call as entry */
  ArrayRef<Type*> argTypes;
  FunctionType *ftype = FunctionType::get(Type::getInt64Ty(TheContext), argTypes, false);
  mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
  BasicBlock *bblock = BasicBlock::Create(TheContext, "entry", mainFunction, 0);

  /* Push a new variable/block context */
  pushBlock(bblock);
  Value *value = root.codeGen(*this); /* emit bytecode for the toplevel block */
  ReturnInst::Create(TheContext, value, bblock);
  popBlock();
  
  /**
   * Print the bytecode in a human-readable format
   * to see if our program compiled properly
   */
  std::cout << "Code is generated.\n";
  verifyModule(*module);
  std::cout << "Code is verified.\n";
  
  legacy::PassManager pm;
  pm.add(createPrintModulePass(outs()));
  pm.run(*module);
  
  cout << "PM has been run.\n";
  }

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
  std::cout << "Running code...\n";
  ExecutionEngine *ee = EngineBuilder(std::move(owner)).create();
  vector<GenericValue> noargs;
  GenericValue v = ee->runFunction(mainFunction, noargs);
  std::cout << "Code was run.\n";
  outs() << "Result: " << v.IntVal << "\n";
  delete ee;
  
  return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type)
{
  if (type.name.compare("int") == 0) {
    return Type::getInt64Ty(TheContext);
  }
  else if (type.name.compare("double") == 0) {
    return Type::getDoubleTy(TheContext);
  }
  return Type::getVoidTy(TheContext);
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context)
{
  std::cout << "Creating integer: " << value << std::endl;
  return ConstantInt::get(Type::getInt64Ty(TheContext), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context)
{
  std::cout << "Creating double: " << value << std::endl;
  return ConstantFP::get(Type::getDoubleTy(TheContext), value);
}

Value* NIdentifier::codeGen(CodeGenContext& context)
{
  std::cout << "Creating identifier reference: " << name << std::endl;
  if (context.locals().find(name) == context.locals().end()) {
    std::cerr << "undeclared variable " << name << std::endl;
    return NULL;
  }
  return context.locals()[name];
//  return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value* NMethodCall::codeGen(CodeGenContext& context)
{
  Function *function = context.getModule()->getFunction(id.name.c_str());
  if (function == NULL) {
    std::cerr << "no such function " << id.name << std::endl;
  }
  std::vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    args.push_back((**it).codeGen(context));
  }
  CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
  std::cout << "Creating method call: " << id.name << std::endl;
  return call;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
  std::cout << "Creating binary operation " << op << std::endl;
  Instruction::BinaryOps instr;
  switch (op) {
    case '+': instr = Instruction::Add; goto math;
    case '-': instr = Instruction::Sub; goto math;
    case '*': instr = Instruction::Mul; goto math;
    case '/': instr = Instruction::SDiv; goto math;
      
      /* TODO comparison */
  }
  
  return NULL;
math:
  return BinaryOperator::Create(instr, lhs.codeGen(context),
                                rhs.codeGen(context), "", context.currentBlock());
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
  std::cout << "Creating assignment for " << lhs.name << std::endl;
  if (context.locals().find(lhs.name) == context.locals().end()) {
    std::cerr << "undeclared variable " << lhs.name << std::endl;
    return NULL;
  }
  return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value* NBlock::codeGen(CodeGenContext& context)
{
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    cout << "Generating code for " << typeid(**it).name() << endl;
    last = (**it).codeGen(context);
  }
  std::cout << "Creating block" << std::endl;
  return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
  std::cout << "Generating code for " << typeid(expression).name() << std::endl;
  return expression.codeGen(context);
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
  std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
  AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
  context.locals()[id.name] = alloc;
  if (assignmentExpr != NULL) {
    NAssignment assn(id, *assignmentExpr);
    assn.codeGen(context);
  }
  return alloc;
  //return new LoadInst(context.locals()[id.name], "", false, context.currentBlock());
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
  std::vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    argTypes.push_back(typeOf((**it).type));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType *ftype = FunctionType::get(typeOf(type), argTypesArray, false);
  Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.getModule());
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
    context.locals()[(**it).id.name] = value;
  }

  Value* value = block.codeGen(context);
  
  cout << "Getting the return value" << endl;
  
  Value *data = new LoadInst(value, "", false, context.currentBlock());
  ReturnInst::Create(TheContext, data, bblock);
  
  context.popBlock();
  std::cout << "Creating function: " << id.name << std::endl;
  return function;
}

