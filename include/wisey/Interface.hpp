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

#include "wisey/IObjectType.hpp"
#include "wisey/MethodSignature.hpp"

namespace wisey {

class InterfaceOwner;
class InterfaceTypeSpecifier;
class MethodSignatureDeclaration;
class Model;
  
/**
 * Contains information about an Interface including the llvm::StructType and method information
 */
class Interface : public IObjectType {
  std::string mName;
  llvm::StructType* mStructType;
  bool mIsExternal;
  InterfaceOwner* mInterfaceOwner;
  std::vector<InterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<MethodSignatureDeclaration *> mMethodSignatureDeclarations;
  std::vector<Interface*> mParentInterfaces;
  std::vector<MethodSignature*> mMethodSignatures;
  std::vector<MethodSignature*> mAllMethodSignatures;
  std::map<IMethodDescriptor*, unsigned long> mMethodIndexes;
  std::map<std::string, MethodSignature*> mNameToMethodSignatureMap;
  bool mIsComplete;
  
public:
  
  ~Interface();

  /**
   * static method for interface instantiation
   */
  static Interface* newInterface(std::string name,
                                 llvm::StructType* structType,
                                 std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                 std::vector<MethodSignatureDeclaration *>
                                 methodSignatureDeclarations);

  /**
   * static method for external interface instantiation
   */
  static Interface* newExternalInterface(std::string name,
                                         llvm::StructType* structType,
                                         std::vector<InterfaceTypeSpecifier*>
                                         parentInterfaceSpecifiers,
                                         std::vector<MethodSignatureDeclaration *>
                                         methodSignatureDeclarations);

  /**
   * Build methods for this interface
   */
  void buildMethods(IRGenerationContext& context);

  /**
   * Generate functions that map interface methods to model methods
   */
  std::vector<std::list<llvm::Constant*>> generateMapFunctionsIR(IRGenerationContext& context,
                                                                 const IObjectType* object,
                                                                 std::map<std::string,
                                                                 llvm::Function*>&
                                                                 methodFunctionMap,
                                                                 unsigned long interfaceIndex) const;
  
  /**
   * Returns parent interfaces
   */
  std::vector<Interface*> getParentInterfaces() const;
  
  /**
   * Tells whether this interface extends a given one
   */
  bool doesExtendInterface(Interface* interface) const;
  
  /**
   * Return function name that casts this interface into a given ICallableObject type
   */
  std::string getCastFunctionName(IObjectType* toType) const;
  
  /**
   * Returns method index in the list of all methods that this interface has or inherits
   */
  unsigned long getMethodIndex(IMethodDescriptor* methodDescriptor) const;
  
  /**
   * Tells whether building of this interface completed
   */
  bool isComplete() const;
 
  /**
   * Defines global variable with the interface name
   */
  void defineInterfaceTypeName(IRGenerationContext& context);

  /**
   * Given a value of type interface get the pointer back to the original object that implements it
   */
  static llvm::Value* getOriginalObject(IRGenerationContext& context, llvm::Value* value);

  MethodSignature* findMethod(std::string methodName) const override;

  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  std::string getShortName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;
  
  const IObjectOwnerType* getOwner() const override;
  
  bool isExternal() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  Interface(std::string name,
            llvm::StructType* structType,
            bool isExternal,
            std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
            std::vector<MethodSignatureDeclaration *> methodSignatureDeclarations);
  
  void includeInterfaceMethods(Interface* parentInterface);
  
  /**
   * Get all method signatures including the ones inherited from parent interfaces
   */
  std::vector<MethodSignature*> getAllMethodSignatures() const;

  llvm::Function* generateMapFunctionForMethod(IRGenerationContext& context,
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
  
  llvm::Function* defineCastFunction(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IObjectType* toType) const;
  
  bool doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                          IMethodDescriptor* objectMethodDescriptor,
                                          std::string objectName) const;
  
};
  
} /* namespace wisey */

#endif /* Interface_hpp */
