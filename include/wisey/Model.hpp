//
//  Model.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
  class Interface;
  class ModelOwner;
  
  /**
   * Contains information about a model including the llvm::StructType and field information
   */
  class Model : public IBuildableConcreteObjectType {
    bool mIsPublic;
    std::string mName;
    llvm::StructType* mStructType;
    bool mIsExternal;
    bool mIsPooled;
    bool mIsInner;
    ModelOwner* mModelOwner;
    std::map<std::string, IField*> mFields;
    std::vector<IField*> mFieldsOrdered;
    std::map<const IField*, unsigned long> mFieldIndexes;
    std::map<IField*, unsigned long> mReceivedFieldIndexes;
    std::vector<IMethod*> mMethods;
    std::map<std::string, IMethod*> mNameToMethodMap;
    std::vector<Interface*> mInterfaces;
    std::vector<Interface*> mFlattenedInterfaceHierarchy;
    std::vector<Constant*> mConstants;
    std::map<std::string, Constant*> mNameToConstantMap;
    std::map<std::string, const IObjectType*> mInnerObjects;
    std::vector<LLVMFunction*> mLLVMFunctions;
    std::map<std::string, LLVMFunction*> mLLVMFunctionMap;
    ImportProfile* mImportProfile;
    int mLine;
    
    Model(AccessLevel accessLevel,
          std::string name,
          llvm::StructType* structType,
          ImportProfile* importProfile,
          bool isExternal,
          bool isPooled,
          int line);

  public:
    
    ~Model();
    
    /**
     * Mmodel instantiation
     */
    static Model* newModel(AccessLevel accessLevel,
                           std::string name,
                           llvm::StructType* structType,
                           ImportProfile* importProfile,
                           int line);

    /**
     * Model instantiation whose instances are allocated from a memory pool
     */
    static Model* newPooledModel(AccessLevel accessLevel,
                                 std::string name,
                                 llvm::StructType* structType,
                                 ImportProfile* importProfile,
                                 int line);

    /**
     * External model instantiation
     */
    static Model* newExternalModel(std::string name,
                                   llvm::StructType* structType,
                                   ImportProfile* importProfile,
                                   int line);
    
    /**
     * External model instantiation whose instances are allocated from a memory pool
     */
    static Model* newPooledExternalModel(std::string name,
                                         llvm::StructType* structType,
                                         ImportProfile* importProfile,
                                         int line);

    /**
     * Gets a set of field names and returns the ones that are missing
     */
    std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;
    
    /**
     * Returns the name of the global variable containing RTTI type representing this model
     */
    std::string getRTTIVariableName() const;
    
    /**
     * Tells whether this model implements a given interface
     */
    bool doesImplmentInterface(Interface* interface) const;
    
    /**
     * Creates a global variable with type description for this model in RTTI format
     */
    void defineRTTI(IRGenerationContext& context) const;
    
    /**
     * Creates a global variable with type description for this model in RTTI format
     */
    void declareRTTI(IRGenerationContext& context) const;

    void setScopeType(const IObjectType* objectType) override;
    
    bool isScopeInjected(IRGenerationContext& context) const override;

    llvm::Function* declareBuildFunction(IRGenerationContext& context) const override;
    
    llvm::Function* defineBuildFunction(IRGenerationContext& context) const override;
    
    bool isPublic() const override;
    
    bool isPooled() const override;

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

    llvm::Instruction* build(IRGenerationContext& context,
                             const ObjectBuilderArgumentList& objectBuilderArgumentList,
                             int line) const override;
    
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

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

    int getLine() const override;
    
    llvm::Constant* getObjectTypeNameGlobal(IRGenerationContext& context) const override;
    
  private:
    
    void checkArguments(IRGenerationContext& context,
                        const ObjectBuilderArgumentList& objectBuilderArgumentList,
                        int line) const;
    
    void checkArgumentsAreWellFormed(IRGenerationContext& context,
                                     const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                     int line) const;
    
    void checkAllFieldsAreSet(IRGenerationContext& context,
                              const ObjectBuilderArgumentList& objectBuilderArgumentList,
                              int line) const;

  };
  
} /* namespace wisey */

#endif /* Model_h */

