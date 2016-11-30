#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

using namespace llvm;
using namespace std;

namespace yazyk {
  
class IStatement;
class IExpression;
class IRGenerationContext;
class VariableDeclaration;

typedef vector<IStatement*> StatementList;
typedef vector<IExpression*> ExpressionList;
typedef vector<VariableDeclaration*> VariableList;

typedef enum PrimitiveTypeEnum {
  PRIMITIVE_TYPE_INT,
  PRIMITIVE_TYPE_LONG,
  PRIMITIVE_TYPE_FLOAT,
  PRIMITIVE_TYPE_DOUBLE
} PrimitiveType;

class INode {
public:
  virtual Value* generateIR(IRGenerationContext& context) = 0;
};

class IExpression : public INode {
};

class IStatement : public INode {
};

class Integer : public IExpression {
public:
  long long value;
  Integer(long long value) : value(value) { }
  Value* generateIR(IRGenerationContext& context);
};

class Double : public IExpression {
public:
  double value;

  Double(double value) : value(value) { }
  Value* generateIR(IRGenerationContext& context);
};

class Identifier : public IExpression {
public:
  string name;

  Identifier(const string& name) : name(name) { }
  Value* generateIR(IRGenerationContext& context);
};

class MethodCall : public IExpression {
public:
  const Identifier& id;
  ExpressionList arguments;

  MethodCall(const Identifier& id, ExpressionList& arguments) :
  id(id), arguments(arguments) { }
  MethodCall(const Identifier& id) : id(id) { }
  Value* generateIR(IRGenerationContext& context);
};

class BinaryOperator : public IExpression {
public:
  int op;
  IExpression& lhs;
  IExpression& rhs;

  BinaryOperator(IExpression& lhs, int op, IExpression& rhs) :
  lhs(lhs), rhs(rhs), op(op) { }
  Value* generateIR(IRGenerationContext& context);
};

class Assignment : public IExpression {
public:
  Identifier& lhs;
  IExpression& rhs;

  Assignment(Identifier& lhs, IExpression& rhs) :
  lhs(lhs), rhs(rhs) { }
  Value* generateIR(IRGenerationContext& context);
};

class Block : public IExpression {
public:
  StatementList statements;

  Block() { }
  Value* generateIR(IRGenerationContext& context);
};

class ExpressionStatement : public IStatement {
public:
  IExpression& expression;

  ExpressionStatement(IExpression& expression) : expression(expression) { }
  Value* generateIR(IRGenerationContext& context);
};

class ReturnStatement : public IStatement {
public:
  IExpression& expression;

  ReturnStatement(IExpression& expression) : expression(expression) { }
  Value* generateIR(IRGenerationContext& context);
};

class TypeSpecifier : public INode {
public:
  PrimitiveType type;

  TypeSpecifier(PrimitiveType type) : type(type) { }
  Value* generateIR(IRGenerationContext& context);
};

class VariableDeclaration : public IStatement {
public:
  const TypeSpecifier& type;
  Identifier& id;
  IExpression *assignmentExpr;

  VariableDeclaration(const TypeSpecifier& type, Identifier& id) :
    type(type), id(id) { assignmentExpr = NULL; }
  VariableDeclaration(const TypeSpecifier& type, Identifier& id, IExpression *assignmentExpr) :
    type(type), id(id), assignmentExpr(assignmentExpr) { }
  Value* generateIR(IRGenerationContext& context);
};

class ExternDeclaration : public IStatement {
public:
  const Identifier& type;
  const Identifier& id;
  VariableList arguments;

  ExternDeclaration(const Identifier& type,
                     const Identifier& id,
                     const VariableList& arguments) :
    type(type), id(id), arguments(arguments) {}
  Value* codeGen(IRGenerationContext& context);
};

class FunctionDeclaration : public IStatement {
public:
  const TypeSpecifier& type;
  const Identifier& id;
  VariableList arguments;
  Block& block;

  FunctionDeclaration(const TypeSpecifier& type,
                         const Identifier& id,
                         const VariableList& arguments,
                         Block& block) :
  type(type), id(id), arguments(arguments), block(block) { }
  Value* generateIR(IRGenerationContext& context);
};

} // namespace yazyk