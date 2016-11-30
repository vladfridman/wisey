#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

using namespace llvm;
using namespace std;

class IYazStatement;
class IYazExpression;
class IRGenerationContext;
class YazVariableDeclaration;

typedef vector<IYazStatement*> YazStatementList;
typedef vector<IYazExpression*> YazExpressionList;
typedef vector<YazVariableDeclaration*> YazVariableList;

typedef enum YazPrimitiveTypeEnum {
  PRIMITIVE_TYPE_INT,
  PRIMITIVE_TYPE_LONG,
  PRIMITIVE_TYPE_FLOAT,
  PRIMITIVE_TYPE_DOUBLE
} YazPrimitiveType;

class IYazNode {
public:
  virtual Value* generateIR(IRGenerationContext& context) = 0;
};

class IYazExpression : public IYazNode {
};

class IYazStatement : public IYazNode {
};

class YazInteger : public IYazExpression {
public:
  long long value;
  YazInteger(long long value) : value(value) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazDouble : public IYazExpression {
public:
  double value;

  YazDouble(double value) : value(value) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazIdentifier : public IYazExpression {
public:
  string name;

  YazIdentifier(const string& name) : name(name) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazMethodCall : public IYazExpression {
public:
  const YazIdentifier& id;
  YazExpressionList arguments;

  YazMethodCall(const YazIdentifier& id, YazExpressionList& arguments) :
  id(id), arguments(arguments) { }
  YazMethodCall(const YazIdentifier& id) : id(id) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazBinaryOperator : public IYazExpression {
public:
  int op;
  IYazExpression& lhs;
  IYazExpression& rhs;

  YazBinaryOperator(IYazExpression& lhs, int op, IYazExpression& rhs) :
  lhs(lhs), rhs(rhs), op(op) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazAssignment : public IYazExpression {
public:
  YazIdentifier& lhs;
  IYazExpression& rhs;

  YazAssignment(YazIdentifier& lhs, IYazExpression& rhs) :
  lhs(lhs), rhs(rhs) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazBlock : public IYazExpression {
public:
  YazStatementList statements;

  YazBlock() { }
  Value* generateIR(IRGenerationContext& context);
};

class YazExpressionStatement : public IYazStatement {
public:
  IYazExpression& expression;

  YazExpressionStatement(IYazExpression& expression) : expression(expression) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazReturnStatement : public IYazStatement {
public:
  IYazExpression& expression;

  YazReturnStatement(IYazExpression& expression) : expression(expression) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazTypeSpecifier : public IYazNode {
public:
  YazPrimitiveType type;

  YazTypeSpecifier(YazPrimitiveType type) : type(type) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazVariableDeclaration : public IYazStatement {
public:
  const YazTypeSpecifier& type;
  YazIdentifier& id;
  IYazExpression *assignmentExpr;

  YazVariableDeclaration(const YazTypeSpecifier& type, YazIdentifier& id) :
    type(type), id(id) { assignmentExpr = NULL; }
  YazVariableDeclaration(const YazTypeSpecifier& type, YazIdentifier& id, IYazExpression *assignmentExpr) :
    type(type), id(id), assignmentExpr(assignmentExpr) { }
  Value* generateIR(IRGenerationContext& context);
};

class YazExternDeclaration : public IYazStatement {
public:
  const YazIdentifier& type;
  const YazIdentifier& id;
  YazVariableList arguments;

  YazExternDeclaration(const YazIdentifier& type,
                     const YazIdentifier& id,
                     const YazVariableList& arguments) :
    type(type), id(id), arguments(arguments) {}
  Value* codeGen(IRGenerationContext& context);
};

class YazFunctionDeclaration : public IYazStatement {
public:
  const YazTypeSpecifier& type;
  const YazIdentifier& id;
  YazVariableList arguments;
  YazBlock& block;

  YazFunctionDeclaration(const YazTypeSpecifier& type,
                         const YazIdentifier& id,
                         const YazVariableList& arguments,
                         YazBlock& block) :
  type(type), id(id), arguments(arguments), block(block) { }
  Value* generateIR(IRGenerationContext& context);
};
