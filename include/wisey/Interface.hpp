//
//  Interface.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <list>
#include <vector>

#include <llvm/IR/Instructions.h>

#include "wisey/IObjectWithMethodsType.hpp"
#include "wisey/MethodSignature.hpp"

namespace wisey {

class MethodSignature;
class Model;
  
/**
 * Contains information about an Interface including the llvm::StructType and method information
 */
class Interface : public IObjectWithMethodsType {
  std::string mName;
  std::string mFullName;
  llvm::StructType* mStructType;
  std::vector<Interface*> mParentInterfaces;
  std::vector<MethodSignature*> mMethodSignatures;
  
  /** 
   * includes inherited method signatures 
   */
  std::vector<MethodSignature*> mAllMethodSignatures;
  std::map<std::string, MethodSignature*> mNameToMethodSignatureMap;
  
public:
  
  Interface(std::string name,
            std::string fullName,
            llvm::StructType* structType,
            std::vector<Interface*> parentInterfaces,
            std::vector<MethodSignature*> methodSignatures);
  
  ~Interface();
  
  /**
   * Generate functions that map interface methods to model methods
   */
  std::vector<std::list<llvm::Constant*>> generateMapFunctionsIR(IRGenerationContext& context,
                                                                 IObjectWithMethodsType* object,
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
   * Return function name that implements instanceof operation
   */
  std::string getInstanceOfFunctionName() const;
  
  /**
   * Return function name that casts this interface into a given ICallableObject type
   */
  std::string getCastFunctionName(IObjectWithMethodsType* toType) const;
  
  /**
   * Call instanceof function and check whether interfaceObject is of type callableObjectType
   */
  llvm::CallInst* callInstanceOf(IRGenerationContext& context,
                                 llvm::Value* interfaceObject,
                                 IObjectWithMethodsType* object) const;
  
  /**
   * Given a value of type interface get the pointer back to the original object that implements it
   */
  static llvm::Value* getOriginalObject(IRGenerationContext& context, llvm::Value* value);
  
  MethodSignature* findMethod(std::string methodName) const override;

  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  std::string getFullName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
  
private:
  
  void prepare();
  
  unsigned long includeInterfaceMethods(Interface* parentInterface,
                                        unsigned long methodIndex);
  
  /**
   * Get all method signatures including the ones inherited from parent interfaces
   */
  std::vector<MethodSignature*> getAllMethodSignatures() const;

  llvm::Function* generateMapFunctionForMethod(IRGenerationContext& context,
                                               IObjectWithMethodsType* object,
                                               llvm::Function* modelFunction,
                                               unsigned long interfaceIndex,
                                               MethodSignature* methodSignature) const;

  void generateMapFunctionBody(IRGenerationContext& context,
                               IObjectWithMethodsType* object,
                               llvm::Function* modelFunction,
                               llvm::Function* mapFunction,
                               unsigned long interfaceIndex,
                               MethodSignature* methodSignature) const;

  llvm::Value* storeArgumentValue(IRGenerationContext& context,
                                  llvm::BasicBlock* basicBlock,
                                  std::string variableName,
                                  IType* variableType,
                                  llvm::Value* variableValue) const;
  
  llvm::Function* defineCastFunction(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IObjectWithMethodsType* toType) const;
  
  bool doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                          IMethodDescriptor* objectMethodDescriptor,
                                          std::string objectName) const;
  
};
  
} /* namespace wisey */

#endif /* Interface_hpp */
