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

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct ModelBuilderTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  NiceMock<MockExpression> mFieldValue3;
  ModelTypeSpecifier mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelBuilderTest() : mModelTypeSpecifier(ModelTypeSpecifier("Shape")) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType *structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    ModelField* widthField = new ModelField(Type::getInt32Ty(llvmContext), 0);
    ModelField* heightField = new ModelField(Type::getInt32Ty(llvmContext), 1);
    (*fields)["width"] = widthField;
    (*fields)["height"] = heightField;
    Model* model = new Model("Shape", structType, fields);
    mContext.addModel(model);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(mFieldValue1, generateIR(_)).WillByDefault(Return(fieldValue1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(mFieldValue2, generateIR(_)).WillByDefault(Return(fieldValue2));
    Value* fieldValue3 = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(mFieldValue3, generateIR(_)).WillByDefault(Return(fieldValue3));

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
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));

  ASSERT_EQ(6ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  string expected = string() +
    "  %malloccall = tail call i8* @malloc(i32 trunc (i64 mul nuw (i64 ptrtoint" +
    " (i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2) to i32))";
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %buildervar = bitcast i8* %malloccall to %Shape*");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = getelementptr %Shape, %Shape* %buildervar, i32 0, i32 0");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 3, i32* %0");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %1 = getelementptr %Shape, %Shape* %buildervar, i32 0, i32 1");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %1");
  mStringBuffer.clear();
}

TEST_F(ModelBuilderTest, InvalidModelBuilderArgumentsDeathTest) {
  Mock::AllowLeak(&mFieldValue1);
  Mock::AllowLeak(&mFieldValue2);
  Mock::AllowLeak(&mFieldValue3);

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
    "\nError: Some arguments for the model 'Shape' builder are not well formed";

  EXPECT_EXIT(modelBuilder.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelBuilderTest, IncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(&mFieldValue1);
  Mock::AllowLeak(&mFieldValue2);
  Mock::AllowLeak(&mFieldValue3);
  
  string argumentSpecifier1("withWidth");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue3);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);
  
  const char *expected =
    "Error: Model builder argumet value for field 'height' does not match its type";
  
  EXPECT_EXIT(modelBuilder.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelBuilderTest, NotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(&mFieldValue1);
  Mock::AllowLeak(&mFieldValue2);
  Mock::AllowLeak(&mFieldValue3);
  
  string argumentSpecifier1("withWidth");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);
  
  const char *expected =
  "Error: Field 'height' is not initialized"
  "\nError: Some fields of the model 'Shape' are not initialized.";
  
  EXPECT_EXIT(modelBuilder.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}

