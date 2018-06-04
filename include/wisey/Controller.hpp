//
//  Controller.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/Interface.hpp"

namespace wisey {
  
  class ControllerOwner;
  
  /**
   * Contains information about a Controller including its fields and methods
   */
  class Controller : public IConcreteObjectType {
    bool mIsPublic;
    std::string mName;
    llvm::StructType* mStructType;
    bool mIsExternal;
    bool mIsInner;
    ControllerOwner* mControllerOwner;
    std::vector<IField*> mReceivedFields;
    std::vector<InjectedField*> mInjectedFields;
    std::vector<IField*> mStateFields;
    std::vector<IMethod*> mMethods;
    std::map<std::string, IField*> mFields;
    std::vector<IField*> mFieldsOrdered;
    std::map<const IField*, unsigned long> mFieldIndexes;
    std::map<IField*, unsigned long> mReceivedFieldIndexes;
    std::map<std::string, IMethod*> mNameToMethodMap;
    std::vector<Interface*> mInterfaces;
    std::vector<Interface*> mFlattenedInterfaceHierarchy;
    std::vector<Constant*> mConstants;
    std::map<std::string, Constant*> mNameToConstantMap;
    std::map<std::string, const IObjectType*> mInnerObjects;
    std::vector<LLVMFunction*> mLLVMFunctions;
    std::map<std::string, LLVMFunction*> mLLVMFunctionMap;
    const IObjectType* mScopeType;
    ImportProfile* mImportProfile;
    int mLine;
    
    Controller(AccessLevel accessLevel,
               std::string name,
               llvm::StructType* structType,
               ImportProfile* importProfile,
               bool isExternal,
               int line);
    
  public:
    
    ~Controller();
    
    /**
     * static method for controller instantiation
     */
    static Controller* newController(AccessLevel accessLevel,
                                     std::string name,
                                     llvm::StructType* structType,
                                     ImportProfile* importProfile,
                                     int line);
    
    /**
     * static method for external controller instantiation
     */
    static Controller* newExternalController(std::string name,
                                             llvm::StructType* structType,
                                             ImportProfile* importProfile,
                                             int line);
    
    /**
     * Returns received fields
     */
    std::vector<IField*> getReceivedFields() const;
    
    /**
     * Creates function that injects this controller
     */
    llvm::Function* createInjectFunction(IRGenerationContext& context, int line) const;
    
    /**
     * Declares function that injects this controller
     */
    llvm::Function* declareInjectFunction(IRGenerationContext& context, int line) const;

    /**
     * Declares injection functions for injected field in this controller
     */
    void declareFieldInjectionFunctions(IRGenerationContext& context, int line) const;
    
    /**
     * Defines field injection functions and schedules composition of their bodies
     */
    void defineFieldInjectorFunctions(IRGenerationContext& context, int line) const;
    
    /**
     * Checks that injection arguments are of correct type and are all there
     */
    void checkInjectionArguments(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const;
    
    /**
     * Checks that injected fields are well formed and have correct arguments
     */
    void checkInjectedFields(IRGenerationContext& context) const;

    void setScopeType(const IObjectType* objectType) override;
    
    bool isScopeInjected(IRGenerationContext& context) const override;

    bool isPublic() const override;
    
    bool isPooled() const override;

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
    
    void setFields(IRGenerationContext& context,
                   std::vector<IField*> fields,
                   unsigned long startIndex) override;
    
    void setInterfaces(std::vector<Interface*> interfaces) override;
    
    void setMethods(std::vector<IMethod*> methods) override;
    
    void setStructBodyTypes(std::vector<llvm::Type*> types) override;
    
    void setConstants(std::vector<Constant*> constants) override;
    
    std::vector<Constant*> getConstants() const override;
    
    void setLLVMFunctions(std::vector<LLVMFunction*> llvmFunctions) override;
    
    std::vector<LLVMFunction*> getLLVMFunctions() const override;
    
    LLVMFunction* findLLVMFunction(std::string functionName) const override;
    
    IField* findField(std::string fieldName) const override;
    
    unsigned long getFieldIndex(const IField* field) const override;
    
    std::vector<IField*> getFields() const override;
    
    const IMethod* findMethod(std::string methodName) const override;
    
    std::map<std::string, IMethod*> getNameToMethodMap() const override;

    Constant* findConstant(IRGenerationContext& context,
                           std::string constantName,
                           int line) const override;
    
    std::vector<IMethod*> getMethods() const override;
    
    std::string getObjectNameGlobalVariableName() const override;
    
    std::string getTypeName() const override;
    
    std::string getShortName() const override;
    
    llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
    
    bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
    
    bool isPrimitive() const override;
    
    bool isOwner() const override;
    
    bool isReference() const override;
    
    bool isArray() const override;
    
    bool isFunction() const override;
    
    bool isPackage() const override;
    
    bool isController() const override;
    
    bool isInterface() const override;
    
    bool isModel() const override;
    
    bool isNode() const override;
    
    bool isNative() const override;
    
    bool isPointer() const override;

    bool isImmutable() const override;

    std::string getVTableName() const override;
     
    std::vector<Interface*> getInterfaces() const override;
    
    std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
    
    std::string getTypeTableName() const override;
    
    const IObjectOwnerType* getOwner() const override;
    
    bool isExternal() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void incrementReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    void decrementReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    llvm::Value* getReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    ImportProfile* getImportProfile() const override;
    
    void addInnerObject(const IObjectType* innerObject) override;
    
    const IObjectType* getInnerObject(std::string shortName) const override;
    
    std::map<std::string, const IObjectType*> getInnerObjects() const override;
    
    void markAsInner() override;
    
    bool isInner() const override;
    
    void createLocalVariable(IRGenerationContext& context,
                             std::string name,
                             int line) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object,
                             int line) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value,
                                 int line) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context, int line) const override;
    
    int getLine() const override;
    
    llvm::Constant* getObjectTypeNameGlobal(IRGenerationContext& context) const override;

  private:
    
    std::vector<std::string> getMissingReceivedFields(std::set<std::string> givenFields) const;
    
    std::vector<InjectedField*> getInjectedFields() const;
    
    void checkArgumentsAreWellFormed(IRGenerationContext& context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const;
    
    void checkAllFieldsAreSet(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const;
    
    void checkReceivedValuesAreForReceivedFields(IRGenerationContext& context,
                                                 const InjectionArgumentList& injectionArgumentList,
                                                 int line) const;

    void initializeReceivedFields(IRGenerationContext& context,
                                  llvm::Function* function,
                                  llvm::Instruction* malloc) const;

    std::string getInjectFunctionName() const;

    static void composeInjectFunctionBody(IRGenerationContext& context,
                                          llvm::Function* function,
                                          const void* objectType);

    static void composeContextInjectFunctionBody(IRGenerationContext& context,
                                                 llvm::Function* function,
                                                 const void* objectType);

    bool isThread(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* Controller_h */

