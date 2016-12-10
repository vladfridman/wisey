#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

namespace yazyk {
  
class IStatement;
class IExpression;
class IRGenerationContext;
class VariableDeclaration;

typedef std::vector<IStatement*> StatementList;
typedef std::vector<IExpression*> ExpressionList;
typedef std::vector<VariableDeclaration*> VariableList;

typedef enum PrimitiveTypeEnum {
  PRIMITIVE_TYPE_INT,
  PRIMITIVE_TYPE_LONG,
  PRIMITIVE_TYPE_FLOAT,
  PRIMITIVE_TYPE_DOUBLE,
  PRIMITIVE_TYPE_VOID,
} PrimitiveType;
  
typedef enum RelationalOperationEnum {
  RELATIONAL_OPERATION_LT,
  RELATIONAL_OPERATION_GT,
  RELATIONAL_OPERATION_LE,
  RELATIONAL_OPERATION_GE,
  RELATIONAL_OPERATION_EQ,
  RELATIONAL_OPERATION_NE,
} RelationalOperation;

class INode {
public:
  virtual ~INode() { }
  virtual llvm::Value* generateIR(IRGenerationContext& context) = 0;
};

class IExpression : public INode {
};

class IStatement : public INode {
};

class Integer : public IExpression {
public:
  long value;
  
  Integer(long value) : value(value) { }
  ~Integer() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Long : public IExpression {
public:
  long long value;

  Long(long long value) : value(value) { }
  ~Long() {}
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Float: public IExpression {
public:
  double value;
  
  Float(double value) : value(value) { }
  ~Float() {}
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Double : public IExpression {
public:
  long double value;

  Double(long double value) : value(value) { }
  ~Double() {}
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Char : public IExpression {
public:
  char value;
  
  Char(char value) : value(value) { }
  ~Char() {}
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class String : public IExpression {
public:
  std::string value;
  
  String(std::string input) : value(unescape(input.substr(1, input.length() - 2))) { }
  ~String() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
  
private:
  std::string unescape(const std::string& input);
};

class Identifier : public IExpression {
public:
  std::string name;

  Identifier(const std::string& name) : name(name) { }
  ~Identifier() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class MethodCall : public IExpression {
public:
  const Identifier& id;
  ExpressionList arguments;

  MethodCall(const Identifier& id, ExpressionList& arguments) :
    id(id), arguments(arguments) { }
  MethodCall(const Identifier& id) : id(id) { }
  ~MethodCall() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
  
private:
  llvm::Function* declarePrintf(IRGenerationContext& context);
};

class AddditiveMultiplicativeExpression : public IExpression {
public:
  IExpression& lhs;
  IExpression& rhs;
  int operation;

  AddditiveMultiplicativeExpression(IExpression& lhs, int operation, IExpression& rhs) :
    lhs(lhs), rhs(rhs), operation(operation) { }
  ~AddditiveMultiplicativeExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class RelationalExpression : public IExpression {
public:
  IExpression& lhs;
  IExpression& rhs;
  RelationalOperation operation;
  
  RelationalExpression(IExpression& lhs, RelationalOperation operation, IExpression& rhs) :
    lhs(lhs), rhs(rhs), operation(operation) { }
  ~RelationalExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class IncrementExpression : public IExpression {
public:
  Identifier identifier;
  long long incrementBy;
  std::string variableName;
  bool isPrefix;

private:
  IncrementExpression(Identifier &identifier,
                      long long incrementBy,
                      std::string variableName,
                      bool isPrefix) :
    identifier(identifier),
    incrementBy(incrementBy),
    variableName(variableName),
    isPrefix(isPrefix) { }
  ~IncrementExpression() { }
  
public:
  llvm::Value* generateIR(IRGenerationContext& context);
  
  static IncrementExpression * newIncrementByOne(Identifier &identifier) {
    return new IncrementExpression(identifier, 1, "inc", false);
  }

  static IncrementExpression * newDecrementByOne(Identifier &identifier) {
    return new IncrementExpression(identifier, -1, "dec", false);
  }
};

class LogicalAndExpression : public IExpression {
public:
  IExpression& lhs;
  IExpression& rhs;
  
  LogicalAndExpression(IExpression& lhs, IExpression& rhs) : lhs(lhs), rhs(rhs) { }
  ~LogicalAndExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class LogicalOrExpression : public IExpression {
public:
  IExpression& lhs;
  IExpression& rhs;
  
  LogicalOrExpression(IExpression& lhs, IExpression& rhs) : lhs(lhs), rhs(rhs) { }
  ~LogicalOrExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};
  
class ConditionalExpression : public IExpression {
public:
  IExpression& conditionExpression;
  IExpression& conditionTrueExpression;
  IExpression& conditionFalseExpression;
  
  ConditionalExpression(IExpression& conditionExpression,
                        IExpression& conditionTrueExpression,
                        IExpression& conditionFalseExpression)
  : conditionExpression(conditionExpression),
    conditionTrueExpression(conditionTrueExpression),
    conditionFalseExpression(conditionFalseExpression) {  }
  ~ConditionalExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Assignment : public IExpression {
public:
  Identifier& lhs;
  IExpression& rhs;

  Assignment(Identifier& lhs, IExpression& rhs) :
    lhs(lhs), rhs(rhs) { }
  ~Assignment() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class Block : public IExpression {
public:
  StatementList statements;

  Block() { }
  ~Block() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class ExpressionStatement : public IStatement {
public:
  IExpression& expression;

  ExpressionStatement(IExpression& expression) : expression(expression) { }
  ~ExpressionStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class ReturnStatement : public IStatement {
public:
  IExpression& expression;

  ReturnStatement(IExpression& expression) : expression(expression) { }
  ~ReturnStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};
  
class ReturnVoidStatement : public IStatement {
public:
  ReturnVoidStatement() { }
  ~ReturnVoidStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class TypeSpecifier : public INode {
public:
  PrimitiveType type;

  TypeSpecifier(PrimitiveType type) : type(type) { }
  ~TypeSpecifier() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
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
  ~VariableDeclaration() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

class ExternDeclaration : public IStatement {
public:
  const Identifier& type;
  const Identifier& id;
  VariableList arguments;

  ExternDeclaration(const Identifier& type,
                     const Identifier& id,
                     const VariableList& arguments) :
    type(type), id(id), arguments(arguments) { }
  ~ExternDeclaration() { }
  
  llvm::Value* codeGen(IRGenerationContext& context);
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
  ~FunctionDeclaration() { }
  
  llvm::Value* generateIR(IRGenerationContext& context);
};

} // namespace yazyk