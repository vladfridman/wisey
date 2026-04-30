//
//  TestIConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IConcreteObjectType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "FakeExpression.hpp"
#include "IConcreteObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IConcreteObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockConcreteObjectType> mMockObject;
  Interface* mInterface3;
  Interface* mCanNavigate;
  Model* mStarModel;
  Model* mGalaxyModel;
  Model* mConstellationModel;
  Model* mCarModel;
  Node* mNode;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IConcreteObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    ON_CALL(mMockObject, getTypeName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName())
    .WillByDefault(Return("Object.typename"));

    vector<Interface*> interfaces;
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    Interface* interface1 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface1",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements,
                                                    mContext.getImportProfile(),
                                                    0);
    Interface* interface2 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface2",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements,
                                                    mContext.getImportProfile(),
                                                    0);
    mInterface3 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                          "Interface3",
                                          NULL,
                                          parentInterfaces,
                                          interfaceElements,
                                          mContext.getImportProfile(),
                                          0);
    Interface* interface4 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface4",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements,
                                                    mContext.getImportProfile(),
                                                    0);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));

    InjectionArgumentList fieldArguments;
    
    vector<Type*> starTypes;
    starTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType* starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<IField*> starFields;
    starFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mBrightness", 0));
    starFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWeight", 0));
    mStarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 starFullName,
                                 starStructType,
                                 mContext.getImportProfile(),
                                 0);
    mStarModel->setFields(mContext, starFields, 1u);
    mContext.addModel(mStarModel, 0);
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mStarModel);
    IConcreteObjectType::defineVTable(mContext, mStarModel);

    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                          ->getPointerTo()->getPointerTo());
    galaxyTypes.push_back(mStarModel->getOwner()->getLLVMType(mContext));
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    vector<IField*> galaxyFields;
    galaxyFields.push_back(new ReceivedField(mStarModel->getOwner(), "mStar", 0));
    mGalaxyModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                   galaxyFullName,
                                   galaxyStructType,
                                   mContext.getImportProfile(),
                                   0);
    mGalaxyModel->setFields(mContext, galaxyFields, 1u);
    mContext.addModel(mGalaxyModel, 0);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mGalaxyModel);
    IConcreteObjectType::defineVTable(mContext, mGalaxyModel);

    vector<Type*> constellationTypes;
    constellationTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                                 ->getPointerTo()->getPointerTo());
    constellationTypes.push_back(mStarModel->getLLVMType(mContext));
    string constellationFullName = "systems.vos.wisey.compiler.tests.MConstellation";
    StructType* constellationStructType = StructType::create(mLLVMContext, constellationFullName);
    constellationStructType->setBody(constellationTypes);
    vector<IField*> constellationFields;
    constellationFields.push_back(new ReceivedField(mStarModel, "mStar", 0));
    mConstellationModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                          constellationFullName,
                                          constellationStructType,
                                          mContext.getImportProfile(),
                                          0);
    mConstellationModel->setFields(mContext, constellationFields, 1u);
    mContext.addModel(mConstellationModel, 0);
    
    vector<Type*> canNavigateTypes;
    canNavigateTypes.push_back(Type::getInt32Ty(mLLVMContext)->getPointerTo()->getPointerTo());
    string canNavigateFullName = "systems.vos.wisey.compiler.tests.ICanNavigate";
    StructType* canNavigateStructType = StructType::create(mLLVMContext, canNavigateFullName);
    vector<IInterfaceTypeSpecifier*> canNavigateParentInterfaces;
    vector<IObjectElementDefinition*> canNavigateElements;
    mCanNavigate = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                           canNavigateFullName,
                                           canNavigateStructType,
                                           canNavigateParentInterfaces,
                                           canNavigateElements,
                                           mContext.getImportProfile(),
                                           0);

    vector<Type*> carTypes;
    carTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                       ->getPointerTo()->getPointerTo());
    carTypes.push_back(mCanNavigate->getLLVMType(mContext));
    string carFullName = "systems.vos.wisey.compiler.tests.MCar";
    StructType* carStructType = StructType::create(mLLVMContext, carFullName);
    carStructType->setBody(carTypes);
    vector<IField*> carFields;
    carFields.push_back(new ReceivedField(mCanNavigate->getOwner(), "mNavigator", 0));
    mCarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                carFullName,
                                carStructType,
                                mContext.getImportProfile(),
                                0);
    mCarModel->setFields(mContext, carFields, 1u);
    mContext.addModel(mCarModel, 0);

    const Controller* cMemoryPool = mContext.getController(Names::getCMemoryPoolFullName(), 0);
    vector<Type*> nodeTypes;
    nodeTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
    nodeTypes.push_back(cMemoryPool->getLLVMType(mContext));
    nodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    nodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStruct = StructType::create(mLLVMContext, nodeFullName);
    nodeStruct->setBody(nodeTypes);
    vector<IField*> nodeFields;
    nodeFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWidth", 0));
    nodeFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mHeight", 0));
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          nodeFullName,
                          nodeStruct,
                          mContext.getImportProfile(),
                          3);
    mNode->setFields(mContext, nodeFields, 2u);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getNamedGlobal("Object.typename"), nullptr);

  IConcreteObjectType::declareTypeNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getNamedGlobal("Object.typename"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, getInterfaceIndexTest) {
  ASSERT_EQ(IConcreteObjectType::getInterfaceIndex(&mMockObject, mInterface3), 2);
}

TEST_F(IConcreteObjectTypeTest, declareFieldVariablesTest) {
  EXPECT_EQ(mContext.getScopes().getVariable("mBrightness"), nullptr);

  IConcreteObjectType::declareFieldVariables(mContext, mStarModel);

  EXPECT_NE(mContext.getScopes().getVariable("mBrightness"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, composeDestructorBodyTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();

  IConcreteObjectType::declareTypeNameGlobal(mContext, mStarModel);
  IConcreteObjectType::defineVTable(mContext, mStarModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mStarModel);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(mStarModel->getTypeName() + ".destructor");
  
  *mStringStream << *function;
  string expected =
  "define void @systems.vos.wisey.compiler.tests.MStar.destructor(ptr %this, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast ptr %this to ptr"
  "\n  %2 = bitcast ptr %1 to ptr"
  "\n  %3 = getelementptr i64, ptr %2, i64 -1"
  "\n  %refCounter = load i64, ptr %3, align 4"
  "\n  %4 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %4, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %5 = getelementptr i8, ptr %this, i64 -8"
  "\n  tail call void @free(ptr %5)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter, ptr @systems.vos.wisey.compiler.tests.MStar.typename, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %6 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %6, ptr %7, align 8"
  "\n  %8 = getelementptr { ptr, i32 }, ptr %7, i32 0, i32 0"
  "\n  %9 = load ptr, ptr %8, align 8"
  "\n  %10 = call ptr @__cxa_get_exception_ptr(ptr %9)"
  "\n  %11 = getelementptr i8, ptr %10, i64 8"
  "\n  resume { ptr, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithObjectOwnerFieldTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();

  IConcreteObjectType::declareTypeNameGlobal(mContext, mStarModel);
  IConcreteObjectType::declareVTable(mContext, mStarModel);
  
  IConcreteObjectType::declareTypeNameGlobal(mContext, mGalaxyModel);
  IConcreteObjectType::defineVTable(mContext, mGalaxyModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mGalaxyModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mGalaxyModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "define void @systems.vos.wisey.compiler.tests.MGalaxy.destructor(ptr %this, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast ptr %this to ptr"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MGalaxy, ptr %1, i32 0, i32 1"
  "\n  %3 = load ptr, ptr %2, align 8"
  "\n  %4 = bitcast ptr %3 to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MStar.destructor(ptr %4, ptr %thread, ptr %callstack, ptr %exception)"
  "\n  %5 = bitcast ptr %1 to ptr"
  "\n  %6 = getelementptr i64, ptr %5, i64 -1"
  "\n  %refCounter = load i64, ptr %6, align 4"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %8 = getelementptr i8, ptr %this, i64 -8"
  "\n  tail call void @free(ptr %8)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter, ptr @systems.vos.wisey.compiler.tests.MGalaxy.typename, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %10 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %9, ptr %10, align 8"
  "\n  %11 = getelementptr { ptr, i32 }, ptr %10, i32 0, i32 0"
  "\n  %12 = load ptr, ptr %11, align 8"
  "\n  %13 = call ptr @__cxa_get_exception_ptr(ptr %12)"
  "\n  %14 = getelementptr i8, ptr %13, i64 8"
  "\n  resume { ptr, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithObjectReferenceFieldTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();

  IConcreteObjectType::declareTypeNameGlobal(mContext, mConstellationModel);
  IConcreteObjectType::defineVTable(mContext, mConstellationModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mConstellationModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mConstellationModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "define void @systems.vos.wisey.compiler.tests.MConstellation.destructor(ptr %this, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast ptr %this to ptr"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MConstellation, ptr %1, i32 0, i32 1"
  "\n  %3 = load ptr, ptr %2, align 8"
  "\n  %4 = icmp eq ptr %3, null"
  "\n  br i1 %4, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %if.this.notnull"
  "\n  %5 = bitcast ptr %1 to ptr"
  "\n  %6 = getelementptr i64, ptr %5, i64 -1"
  "\n  %refCounter = load i64, ptr %6, align 4"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nif.notnull:                                       ; preds = %if.this.notnull"
  "\n  %8 = bitcast ptr %3 to ptr"
  "\n  %9 = getelementptr i64, ptr %8, i64 -1"
  "\n  %10 = atomicrmw add ptr %9, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.end"
  "\n  %11 = getelementptr i8, ptr %this, i64 -8"
  "\n  tail call void @free(ptr %11)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.end"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter, ptr @systems.vos.wisey.compiler.tests.MConstellation.typename, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %12 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %13 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %12, ptr %13, align 8"
  "\n  %14 = getelementptr { ptr, i32 }, ptr %13, i32 0, i32 0"
  "\n  %15 = load ptr, ptr %14, align 8"
  "\n  %16 = call ptr @__cxa_get_exception_ptr(ptr %15)"
  "\n  %17 = getelementptr i8, ptr %16, i64 8"
  "\n  resume { ptr, i32 } %12"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithInterfaceOwnerFieldsTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();

  IConcreteObjectType::declareTypeNameGlobal(mContext, mCarModel);
  IConcreteObjectType::defineVTable(mContext, mCarModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mCarModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mCarModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "define void @systems.vos.wisey.compiler.tests.MCar.destructor(ptr %this, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast ptr %this to ptr"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MCar, ptr %1, i32 0, i32 1"
  "\n  %3 = load ptr, ptr %2, align 8"
  "\n  %4 = bitcast ptr %3 to ptr"
  "\n  call void @__destroyObjectOwnerFunction(ptr %4, ptr %thread, ptr %callstack, ptr %exception)"
  "\n  %5 = bitcast ptr %1 to ptr"
  "\n  %6 = getelementptr i64, ptr %5, i64 -1"
  "\n  %refCounter = load i64, ptr %6, align 4"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %8 = getelementptr i8, ptr %this, i64 -8"
  "\n  tail call void @free(ptr %8)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter, ptr @systems.vos.wisey.compiler.tests.MCar.typename, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %10 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %9, ptr %10, align 8"
  "\n  %11 = getelementptr { ptr, i32 }, ptr %10, i32 0, i32 0"
  "\n  %12 = load ptr, ptr %11, align 8"
  "\n  %13 = call ptr @__cxa_get_exception_ptr(ptr %12)"
  "\n  %14 = getelementptr i8, ptr %13, i64 8"
  "\n  resume { ptr, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorCallTest) {
  IConcreteObjectType::declareTypeNameGlobal(mContext, mStarModel);
  IConcreteObjectType::declareVTable(mContext, mStarModel);

  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "main",
                                        mContext.getModule());
  
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);

  ConstantPointerNull* objectPointer = ConstantPointerNull::get(mCarModel->getLLVMType(mContext));
  llvm::PointerType* int8Pointer = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* nullPointer = ConstantPointerNull::get(int8Pointer);
  IConcreteObjectType::composeDestructorCall(mContext, mStarModel, objectPointer, nullPointer, 0);
  
  *mStringStream << *basicBlock;
  string expected =
  ""
  "\nentry:"
  "\n  %0 = bitcast ptr null to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MStar.destructor(ptr %0, ptr null, ptr null, ptr null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorBodyNodeObjectTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  IConcreteObjectType::declareTypeNameGlobal(mContext, mNode);
  IConcreteObjectType::defineVTable(mContext, mNode);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mNode);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(mNode->getTypeName() + ".destructor");
  
  *mStringStream << *function;
  string expected =
  "define void @systems.vos.wisey.compiler.tests.NNode.destructor(ptr %this, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast ptr %this to ptr"
  "\n  %2 = bitcast ptr %1 to ptr"
  "\n  %3 = getelementptr i64, ptr %2, i64 -1"
  "\n  %refCounter = load i64, ptr %3, align 4"
  "\n  %4 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %4, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %5 = getelementptr %systems.vos.wisey.compiler.tests.NNode, ptr %1, i32 0, i32 1"
  "\n  %6 = load ptr, ptr %5, align 8"
  "\n  %7 = icmp eq ptr %6, null"
  "\n  br i1 %7, label %pool.is.null, label %pool.is.notnull"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter, ptr @systems.vos.wisey.compiler.tests.NNode.typename, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %8 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %9 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %8, ptr %9, align 8"
  "\n  %10 = getelementptr { ptr, i32 }, ptr %9, i32 0, i32 0"
  "\n  %11 = load ptr, ptr %10, align 8"
  "\n  %12 = call ptr @__cxa_get_exception_ptr(ptr %11)"
  "\n  %13 = getelementptr i8, ptr %12, i64 8"
  "\n  resume { ptr, i32 } %8"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n"
  "\npool.is.null:                                     ; preds = %ref.count.zero"
  "\n  %14 = getelementptr i8, ptr %this, i64 -8"
  "\n  tail call void @free(ptr %14)"
  "\n  ret void"
  "\n"
  "\npool.is.notnull:                                  ; preds = %ref.count.zero"
  "\n  %15 = bitcast ptr %6 to ptr"
  "\n  %16 = getelementptr %CMemoryPool, ptr %15, i32 0, i32 1"
  "\n  %17 = load i64, ptr %16, align 4"
  "\n  %18 = sub i64 %17, 1"
  "\n  store i64 %18, ptr %16, align 4"
  "\n  %19 = icmp eq i64 %18, 0"
  "\n  br i1 %19, label %pool.count.zero, label %pool.count.notzero"
  "\n"
  "\npool.count.zero:                                  ; preds = %pool.is.notnull"
  "\n  %20 = getelementptr %CMemoryPool, ptr %15, i32 0, i32 2"
  "\n  %21 = load ptr, ptr %20, align 8"
  "\n  call void @mem_pool_clear(ptr %21)"
  "\n  br label %pool.count.notzero"
  "\n"
  "\npool.count.notzero:                               ; preds = %pool.count.zero, %pool.is.notnull"
  "\n  ret void"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, addInterfaceAndItsParentsTest) {
  vector<IInterfaceTypeSpecifier*> interfaceTypeSpecifiers;
  vector<IObjectElementDefinition *> objectElements;

  vector<Interface*> flattenedInterfaces;

  string package = "some.package";
  StructType* grandChildStructType = StructType::create(mLLVMContext, "some.package.IGrandChild");
  Interface* grandChild = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                  "some.package.IGrandChild",
                                                  grandChildStructType,
                                                  interfaceTypeSpecifiers,
                                                  objectElements,
                                                  mContext.getImportProfile(),
                                                  0);

  PackageType* packageType = new PackageType(package);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression,
                                                               "IGrandChild",
                                                               0));
  StructType* child1StructType = StructType::create(mLLVMContext, "some.package.IChild1");
  Interface* child1 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IChild1",
                                              child1StructType,
                                              interfaceTypeSpecifiers,
                                              objectElements,
                                              mContext.getImportProfile(),
                                              0);
  interfaceTypeSpecifiers.clear();
  StructType* child2StructType = StructType::create(mLLVMContext, "some.package.IChild2");
  Interface* child2 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IChild2",
                                              child2StructType,
                                              interfaceTypeSpecifiers,
                                              objectElements,
                                              mContext.getImportProfile(),
                                              0);

  packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression, "IChild1", 0));
  packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression, "IChild2", 0));
  StructType* parentStructType = StructType::create(mLLVMContext, "some.package.IParent");
  Interface* parent = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IParent",
                                              parentStructType,
                                              interfaceTypeSpecifiers,
                                              objectElements,
                                              mContext.getImportProfile(),
                                              0);
  mContext.addInterface(parent, 0);
  mContext.addInterface(child1, 0);
  mContext.addInterface(child2, 0);
  mContext.addInterface(grandChild, 0);

  parent->buildMethods(mContext);

  IConcreteObjectType::addInterfaceAndItsParents(parent, flattenedInterfaces);
  
  EXPECT_EQ(flattenedInterfaces.size(), 4u);
  EXPECT_EQ(flattenedInterfaces.at(0), parent);
  EXPECT_EQ(flattenedInterfaces.at(1), child1);
  EXPECT_EQ(flattenedInterfaces.at(2), grandChild);
  EXPECT_EQ(flattenedInterfaces.at(3), child2);
}

TEST_F(TestFileRunner, freeingFieldVariablesRunTest) {
  runFile("tests/samples/test_freeing_field_variables.yz", 3);
}

TEST_F(TestFileRunner, freeingFieldInterfaceOwnersRunTest) {
  runFile("tests/samples/test_freeing_field_interface_owners.yz", 3);
}

TEST_F(TestFileRunner, compareObjectToNullRunTest) {
  runFile("tests/samples/test_compare_object_to_null.yz", 1);
}

TEST_F(TestFileRunner, objectMethodMarkedErroneouslyOverrideRunDeathTest) {
  expectFailCompile("tests/samples/test_object_method_marked_erroneously_override.yz",
                    1,
                    "tests/samples/test_object_method_marked_erroneously_override.yz\\(5\\): "
                    "Error: Method 'getValue' of object systems.vos.wisey.compiler.tests.MMyModel "
                    "is marked override but does not override any interface methods");
}

TEST_F(TestFileRunner, overloadGetTypeNameRunDeathTest) {
  expectFailCompile("tests/samples/test_overload_get_type_name.yz",
                    1,
                    "tests/samples/test_overload_get_type_name.yz\\(14\\): Error: "
                    "Method named 'getTypeName' is reserved and can not be overloaded");
}
