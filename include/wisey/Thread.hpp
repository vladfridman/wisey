//
//  Thread.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef Thread_h
#define Thread_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IInjectable.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/ObjectBuilderArgument.hpp"
#include "wisey/PointerType.hpp"

namespace wisey {
  
  class ThreadOwner;
  
  /**
   * Contains information about a Thread object including its fields and methods
   */
  class Thread : public IConcreteObjectType, public IInjectable {
    AccessLevel mAccessLevel;
    std::string mName;
    llvm::StructType* mStructType;
    bool mIsExternal;
    bool mIsInner;
    ThreadOwner* mThreadOwner;
    const PointerType* mPointerType;
    std::map<std::string, IField*> mFields;
    std::vector<IField*> mReceivedFields;
    std::vector<InjectedField*> mInjectedFields;
    std::vector<IField*> mStateFields;
    std::vector<IField*> mFieldsOrdered;
    std::map<IField*, unsigned long> mFieldIndexes;
    std::vector<IMethod*> mMethods;
    std::map<std::string, IMethod*> mNameToMethodMap;
    std::vector<Interface*> mInterfaces;
    std::vector<Interface*> mFlattenedInterfaceHierarchy;
    std::vector<Constant*> mConstants;
    std::map<std::string, Constant*> mNameToConstantMap;
    std::map<std::string, const IObjectType*> mInnerObjects;
    ImportProfile* mImportProfile;
    
  public:
    
    ~Thread();
    
    /**
     * static method for thread instantiation
     */
    static Thread* newThread(AccessLevel accessLevel,
                             std::string name,
                             llvm::StructType* structType);
    
    /**
     * static method for external thread instantiation
     */
    static Thread* newExternalThread(std::string name, llvm::StructType* structType);
    
    AccessLevel getAccessLevel() const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
    
    void setFields(std::vector<IField*> fields, unsigned long startIndex) override;
    
    void setInterfaces(std::vector<Interface*> interfaces) override;
    
    void setMethods(std::vector<IMethod*> methods) override;
    
    void setStructBodyTypes(std::vector<llvm::Type*> types) override;
    
    void setConstants(std::vector<Constant*> constants) override;
    
    std::vector<Constant*> getConstants() const override;
    
    IField* findField(std::string fieldName) const override;
    
    unsigned long getFieldIndex(IField* field) const override;
    
    std::vector<IField*> getFields() const override;
    
    IMethod* findMethod(std::string methodName) const override;
    
    Constant* findConstant(std::string constantName) const override;
    
    std::vector<IMethod*> getMethods() const override;
    
    std::string getObjectNameGlobalVariableName() const override;
    
    std::string getObjectShortNameGlobalVariableName() const override;
    
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
    
    bool isThread() const override;
    
    bool isNative() const override;

    std::string getVTableName() const override;
    
    unsigned long getVTableSize() const override;
    
    std::vector<Interface*> getInterfaces() const override;
    
    std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
    
    std::string getTypeTableName() const override;
    
    const IObjectOwnerType* getOwner() const override;
    
    bool isExternal() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void incrementReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    void decrementReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    llvm::Value* getReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
    
    void setImportProfile(ImportProfile* importProfile) override;
    
    ImportProfile* getImportProfile() const override;
    
    void addInnerObject(const IObjectType* innerObject) override;
    
    const IObjectType* getInnerObject(std::string shortName) const override;
    
    std::map<std::string, const IObjectType*> getInnerObjects() const override;
    
    void markAsInner() override;
    
    bool isInner() const override;
    
    llvm::Function* getReferenceAdjustmentFunction(IRGenerationContext& context) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;
    
    const Thread* getObjectType() const override;
    
    const IType* getPointerType() const override;
    
    const IType* getDereferenceType() const override;

  private:
    
    Thread(AccessLevel accessLevel,
           std::string name,
           llvm::StructType* structType,
           bool isExternal);
    
    void checkArguments(const InjectionArgumentList& injectionArgumentList) const;
    
    void checkArgumentsAreWellFormed(const InjectionArgumentList& injectionArgumentList) const;
    
    void checkAllFieldsAreSet(const InjectionArgumentList& injectionArgumentList) const;
    
    std::vector<std::string> getMissingReceivedFields(std::set<std::string> givenFields) const;
    
    llvm::Instruction* createMalloc(IRGenerationContext& context) const;
    
    void initializeReceivedFields(IRGenerationContext& context,
                                  const InjectionArgumentList& controllerInjectorArguments,
                                  llvm::Instruction* malloc,
                                  int line) const;

    void initializeInjectedFields(IRGenerationContext& context,
                                  llvm::Instruction* malloc,
                                  int line) const;
    
  };

} /* namespace wisey */

#endif /* Thread_h */
