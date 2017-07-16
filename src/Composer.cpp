//
//  Composer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/RelationalExpression.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Composer::checkNullAndThrowNPE(IRGenerationContext& context, Value* value, const IType* type) {
  FakeExpression* fakeExpression = new FakeExpression(value, type);
  NullExpression* nullExpression = new NullExpression();
  RelationalExpression* relationalExpression = new RelationalExpression(fakeExpression,
                                                                        RELATIONAL_OPERATION_EQ,
                                                                        nullExpression);
  Block* thenBlock = new Block();
  
  vector<string> package;
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(package, "MNullPointerException");
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier, objectBuilderArgumnetList);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder);
  thenBlock->getStatements().push_back(throwStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock);
  IfStatement ifStatement(relationalExpression, thenStatement);
  
  ifStatement.generateIR(context);
}

