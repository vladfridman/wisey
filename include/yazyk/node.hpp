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

} // namespace yazyk

#endif /* node_hpp */
