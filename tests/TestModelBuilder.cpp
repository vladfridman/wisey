//
//  TestModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelBuilder}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelBuilder.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/ModelFieldDeclaration.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using ::testing::NiceMock;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct ModelBuilderTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  ModelTypeSpecifier mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelBuilderTest() : mModelTypeSpecifier(ModelTypeSpecifier("Shape")) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType *structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, Type*>* fields = new map<string, Type*>();
    Model* model = new Model(structType, fields);
    mContext.addModel("model.Shape", model);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ModelBuilderTest() {
    delete mStringStream;
  }
};

TEST_F(ModelBuilderTest, ValidModelBuilderArgumentsTest) {
  string argumentSpecifier1("withWidth");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue2);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);

  Value* result = modelBuilder.generateIR(mContext);
  
  EXPECT_TRUE(result != NULL);
  EXPECT_TRUE(BitCastInst::classof(result));

  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  string expected = string() +
    "  %malloccall = tail call i8* @malloc(i32 ptrtoint (i32* getelementptr (i32, i32*"
    " null, i32 1) to i32))";
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %buildervar = bitcast i8* %malloccall to %Shape*");
  mStringBuffer.clear();
}

TEST_F(ModelBuilderTest, InvalidModelBuilderArgumentsDeathTest) {
  string argumentSpecifier1("width");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue2);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);
  
  const char *expected =
    "Error: Model builder argument should start with 'with'. e.g. .withField\\(value\\)."
    "\nError: Some arguments for the model builder are not well formed";

  EXPECT_EXIT(modelBuilder.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}
