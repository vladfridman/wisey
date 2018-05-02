//
//  Interface.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <iostream>
#include <list>
#include <vector>

#include <llvm/IR/Instructions.h>

#include "wisey/Constant.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/InjectionArgument.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/StaticMethod.hpp"

namespace wisey {
  
  class Controller;
  class IInterfaceTypeSpecifier;
  class InterfaceOwner;
  class LLVMFunction;
  class MethodSignatureDeclaration;
  class Model;
  
  /**
   * Contains information about an Interface including the llvm::StructType and method information
   */
  class Interface : public IObjectType {
    bool mIsPublic;
    std::string mName;
    llvm::StructType* mStructType;
    bool mIsExternal;
    bool mIsInner;
    InterfaceOwner* mInterfaceOwner;
    std::vector<IInterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
    std::vector<IObjectElementDefinition *> mElementDeclarations;
    std::vector<Interface*> mParentInterfaces;
    std::map<std::string, Interface*> mParentInterfacesMap;
    std::vector<MethodSignature*> mMethodSignatures;
    std::vector<MethodSignature*> mAllMethodSignatures;
    std::vector<StaticMethod*> mStaticMethods;
    std::map<const IMethodDescriptor*, unsigned long> mMethodIndexes;
    std::map<std::string, MethodSignature*> mNameToMethodSignatureMap;
    std::map<std::string, StaticMethod*> mNameToStaticMethodMap;
    std::vector<wisey::Constant*> mConstants;
    std::map<std::string, Constant*> mNameToConstantMap;
    std::map<std::string, const IObjectType*> mInnerObjects;
    std::vector<LLVMFunction*> mLLVMFunctions;
    std::map<std::string, LLVMFunction*> mLLVMFunctionMap;
    ImportProfile* mImportProfile;
    bool mIsComplete;
    int mLine;
    
    Interface(AccessLevel accessLevel,
              std::string name,
              llvm::StructType* structType,
              bool isExternal,
              std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
              std::vector<IObjectElementDefinition *> elementDelcarations,
              ImportProfile* importProfile,
              int line);
    
  public:
    
    ~Interface();
    
    /**
     * static method for interface instantiation
     */
    static Interface* newInterface(AccessLevel accessLevel,
                                   std::string name,
                                   llvm::StructType* structType,
                                   std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                   std::vector<IObjectElementDefinition *> elementDeclarations,
                                   ImportProfile* importProfile,
                                   int line);
    
    /**
     * static method for external interface instantiation
     */
    static Interface* newExternalInterface(std::string name,
                                           llvm::StructType* structType,
                                           std::vector<IInterfaceTypeSpecifier*>
                                           parentInterfaceSpecifiers,
                                           std::vector<IObjectElementDefinition *>
                                           elementDeclarations,
                                           ImportProfile* importProfile,
                                           int line);
    
    /**
     * Build methods for this interface
     */
    void buildMethods(IRGenerationContext& context);
    
    /**
     * Define functions that map interface methods to model methods
     */
    std::vector<std::list<llvm::Constant*>> defineMapFunctions(IRGenerationContext& context,
                                                               const IObjectType* object,
                                                               std::map<std::string,
                                                               llvm::Function*>& methodFunctionMap,
                                                               unsigned long interfaceIndex) const;
    /**
     * Compose functions that map interface methods to model methods
     */
    unsigned long composeMapFunctions(IRGenerationContext& context,
                                      const IObjectType* object,
                                      unsigned long interfaceIndex) const;
    
    /**
     * Returns parent interfaces
     */
    std::vector<Interface*> getParentInterfaces() const;
    
    /**
     * Tells whether this interface extends a given one
     */
    bool doesExtendInterface(const Interface* interface) const;
    
    /**
     * Return function name that casts this interface into a given ICallableObject type
     */
    std::string getCastFunctionName(const IObjectType* toType) const;
    
    /**
     * Returns method index in the list of all methods that this interface has or inherits
     */
    unsigned long getMethodIndex(const IMethodDescriptor* methodDescriptor) const;
    
    /**
     * Tells whether building of this interface completed
     */
    bool isComplete() const;
    
    /**
     * Defines global variable with the interface name
     */
    void defineInterfaceTypeName(IRGenerationContext& context) const;
    
    /**
     * Defines global containing pointer to the function that injects the interface
     */
    void defineInjectionFunctionPointer(IRGenerationContext& context) const;
    
    /**
     * Defines external global containing pointer to the function that injects the interface
     */
    void defineExternalInjectionFunctionPointer(IRGenerationContext& context) const;
    
    /**
     * Defines llvm functions corresponding to interface static methods
     */
    void defineStaticMethodFunctions(IRGenerationContext& context) const;
    
    /**
     * Defines llvm functions corresponding for llvm functions defined in the interface
     */
    void defineLLVMFunctions(IRGenerationContext& context) const;

    /**
     * Generate IR for constants defined in this interface
     */
    void generateConstantsIR(IRGenerationContext& context) const;
    
    /**
     * Define variable containing interface's name
     */
    void defineCurrentObjectNameVariable(IRGenerationContext& context) const;
    
    /**
     * Generates code for static methods defined in this interface
     */
    void generateStaticMethodsIR(IRGenerationContext& context) const;
    
    /**
     * Generates code for llvm functions defined in this interface
     */
    void generateLLVMFunctionsIR(IRGenerationContext& context) const;

    /**
     * Looks up a static method defined in this interface
     */
    IMethod* findStaticMethod(std::string name) const;
    
    /**
     * Inject the object this interface is bound to
     */
    llvm::Value* inject(IRGenerationContext& context,
                        const InjectionArgumentList injectionArgumentList,
                        int line) const override;
    
    /**
     * Composes inject function for this interface that injects the given controller
     */
    llvm::Value* composeInjectFunctionWithController(IRGenerationContext& context,
                                                     const Controller* controller) const;
    
    bool isPublic() const override;

    IMethodDescriptor* findMethod(std::string methodName) const override;
    
    Constant* findConstant(IRGenerationContext& context,
                           std::string constantName,
                           int line) const override;

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
    
    llvm::Function* getReferenceAdjustmentFunction(IRGenerationContext& context) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;
    
    LLVMFunction* findLLVMFunction(std::string functionName) const override;
    
    int getLine() const override;

  private:
    
    void processParentInterfaces(IRGenerationContext& context);
    
    void processMethodSignatures(IRGenerationContext& context);
    
    void includeInterfaceMethods(Interface* parentInterface,
                                 std::set<std::string>& methodOverrides);
    
    /**
     * Get all method signatures including the ones inherited from parent interfaces
     */
    std::vector<MethodSignature*> getAllMethodSignatures() const;
    
    llvm::Function* defineMapFunctionForMethod(IRGenerationContext& context,
                                               const IObjectType* object,
                                               llvm::Function* concreteObjectFunction,
                                               unsigned long interfaceIndex,
                                               MethodSignature* methodSignature) const;
    
    void composeMapFunctionBody(IRGenerationContext& context,
                                const IObjectType* object,
                                llvm::Function* concreteObjectFunction,
                                unsigned long interfaceIndex,
                                MethodSignature* methodSignature) const;
    
    void generateMapFunctionBody(IRGenerationContext& context,
                                 const IObjectType* object,
                                 llvm::Function* concreteObjectFunction,
                                 llvm::Function* mapFunction,
                                 unsigned long interfaceIndex,
                                 MethodSignature* methodSignature) const;
    
    llvm::Value* storeArgumentValue(IRGenerationContext& context,
                                    llvm::BasicBlock* basicBlock,
                                    std::string variableName,
                                    const IType* variableType,
                                    llvm::Value* variableValue) const;
    
    bool doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                            IMethodDescriptor* objectMethodDescriptor,
                                            std::string objectName) const;
    
    std::tuple<std::vector<MethodSignature*>, std::vector<wisey::Constant*>,
    std::vector<StaticMethod*>, std::vector<LLVMFunction*>>
    createElements(IRGenerationContext& context,
                   std::vector<IObjectElementDefinition*>
                   elementDeclarations);
    
    llvm::Function* getOrCreateEmptyInjectFunction(IRGenerationContext& context, int line) const;
    
    static void composeEmptyInjectFunction(IRGenerationContext& context,
                                           llvm::Function* function,
                                           const void* object);

    static void composeInjectWithControllerFunction(IRGenerationContext& context,
                                                    llvm::Function* function,
                                                    const void* object1,
                                                    const void* object2);
    
    std::string getInjectWrapperFunctionName() const;
    
    std::string getInjectFunctionVariableName() const;
    
    std::string getInjectFunctionName() const;

  };
  
} /* namespace wisey */

#endif /* Interface_hpp */

