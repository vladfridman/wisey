#include "node.hpp"
#include "codegen.hpp"
#include "y.tab.h"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root) {
  cout << "Generating code...\n";
  
  owner = make_unique<Module>("test", TheContext);
  module = owner.get();
  
  /* Create the top level interpreter function to call as entry */
  ArrayRef<Type*> argTypes;
  FunctionType *ftype = FunctionType::get(Type::getInt32Ty(TheContext), argTypes, false);
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
  cout << "Code is generated.\n";
  verifyModule(*module);
  cout << "Code is verified.\n";
  
  legacy::PassManager pm;
  pm.add(createPrintModulePass(outs()));
  pm.run(*module);
  
  cout << "PM has been run.\n";
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
  cout << "Running code...\n";
  ExecutionEngine *ee = EngineBuilder(move(owner)).create();
  vector<GenericValue> noargs;
  GenericValue v = ee->runFunction(mainFunction, noargs);
  cout << "Code was run.\n";
  outs() << "Result: " << v.IntVal << "\n";
  delete ee;
  
  return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NTypeSpecifier& type) {
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

Value* NInteger::codeGen(CodeGenContext& context) {
  cout << "Creating integer: " << value << endl;
  return ConstantInt::get(Type::getInt32Ty(TheContext), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context) {
  cout << "Creating double: " << value << endl;
  return ConstantFP::get(Type::getDoubleTy(TheContext), value);
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
  cout << "Creating identifier reference: " << name << endl;
  if (context.locals().find(name) == context.locals().end()) {
    cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[name], "", context.currentBlock());
}

Value * NTypeSpecifier::codeGen(CodeGenContext &context) {
  return NULL;
}

Value* NMethodCall::codeGen(CodeGenContext& context) {
  Function *function = context.getModule()->getFunction(id.name.c_str());
  if (function == NULL) {
    cerr << "no such function " << id.name << endl;
  }
  vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    args.push_back((**it).codeGen(context));
  }
  CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
  cout << "Creating method call: " << id.name << endl;
  return call;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context) {
  cout << "Creating binary operation " << op << endl;
  Instruction::BinaryOps instr;
  switch (op) {
    case '+': instr = Instruction::Add; break;
    case '-': instr = Instruction::Sub; break;
    case '*': instr = Instruction::Mul; break;
    case '/': instr = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value * lhsValue = lhs.codeGen(context);
  Value * rhsValue = rhs.codeGen(context);

  return BinaryOperator::Create(instr,
                                lhsValue,
                                rhsValue,
                                "",
                                context.currentBlock());
}

Value* NAssignment::codeGen(CodeGenContext& context) {
  cout << "Creating assignment for " << lhs.name << endl;
  if (context.locals().find(lhs.name) == context.locals().end()) {
    cerr << "undeclared variable " << lhs.name << endl;
    return NULL;
  }
  return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], context.currentBlock());
}

Value* NBlock::codeGen(CodeGenContext& context) {
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    NStatement *statement = *it;
    cout << "Generating block code for " << typeid(*statement).name() << endl;
    last = statement->codeGen(context);
  }
  cout << "Creating block" << endl;
  return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context) {
  cout << "Generating expression statement code for " << typeid(expression).name() << endl;
  return expression.codeGen(context);
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context) {
  cout << "Creating variable declaration " << type.type << " " << id.name << endl;
  AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
  context.locals()[id.name] = alloc;
  if (assignmentExpr != NULL) {
    NAssignment assn(id, *assignmentExpr);
    assn.codeGen(context);
  }
  return alloc;
}


Value* NReturnStatement::codeGen(CodeGenContext& context) {
  cout << "Generatring return statement" << endl;

  Value * result = ReturnInst::Create(TheContext, expression.codeGen(context), context.currentBlock());
  return result;
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context) {
  vector<Type*> argTypes;
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
    string newName = (**it).id.name + ".param";
    AllocaInst *alloc = new AllocaInst(typeOf((**it).type), newName, bblock);
    value = new StoreInst(value, alloc, bblock);
    context.locals()[(**it).id.name] = alloc;
  }

  block.codeGen(context);
  
  context.popBlock();
  cout << "Creating function: " << id.name << endl;
  return function;
}

