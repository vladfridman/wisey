#ifndef node_hpp
#define node_hpp

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

#include "yazyk/INode.hpp"

namespace yazyk {
  
class Identifier;
class IStatement;
class IExpression;
class IRGenerationContext;
class VariableDeclaration;

typedef std::vector<IStatement*> StatementList;
typedef std::vector<IExpression*> ExpressionList;
typedef std::vector<VariableDeclaration*> VariableList;

typedef enum RelationalOperationEnum {
  RELATIONAL_OPERATION_LT,
  RELATIONAL_OPERATION_GT,
  RELATIONAL_OPERATION_LE,
  RELATIONAL_OPERATION_GE,
  RELATIONAL_OPERATION_EQ,
  RELATIONAL_OPERATION_NE,
} RelationalOperation;


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

} // namespace yazyk

#endif /* node_hpp */
