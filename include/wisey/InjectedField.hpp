//
//  InjectedField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef InjectedField_h
#define InjectedField_h

#include "wisey/IField.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Controller;
  class IObjectType;
  class Interface;
  
  /**
   * Represents an injected field in a concrete object
   */
  class InjectedField : public IField {
    const IType* mType;
    const IType* mInjectedType;
    bool mIsArrayType;
    std::string mName;
    InjectionArgumentList mInjectionArgumentList;
    std::string mSourceFileName;
    bool mIsImmediate;
    int mLine;
    
    InjectedField(const IType* type,
                  const IType* injectedType,
                  std::string name,
                  InjectionArgumentList injectionArgumentList,
                  std::string sourceFileName,
                  bool isImmediate,
                  int line);

  public:
    
    ~InjectedField();
    
    /**
     * Creates an instance of InjectedField that is delayed injected
     */
    static InjectedField* createDelayed(const IType* type,
                                        const IType* injectedType,
                                        std::string name,
                                        InjectionArgumentList injectionArguments,
                                        std::string sourceFile,
                                        int line);
    
    /**
     * Creates an instance of InjectedField that is delayed injected
     */
    static InjectedField* createImmediate(const IType* type,
                                          const IType* injectedType,
                                          std::string name,
                                          InjectionArgumentList injectionArguments,
                                          std::string sourceFile,
                                          int line);

    /**
     * Injects the field type and returns the injected value
     */
    llvm::Value* inject(IRGenerationContext& context) const;
    
    /**
     * Declares the function that injects this field
     */
    llvm::Function* declareInjectionFunction(IRGenerationContext& context,
                                             const Controller* controller) const;
    /**
     * Defines function that injects this field and schedules its body composition
     */
    void defineInjectionFunction(IRGenerationContext& context, const Controller* controller) const;

    /**
     * Returns the name of the function that injects this field
     */
    std::string getInjectionFunctionName(const IConcreteObjectType* controller) const;
    
    /**
     * Checks that field injected type is of object owner or array owner type
     */
    void checkType(IRGenerationContext& context) const;
    
    /**
     * Checks that injection arguments are all there and match received types
     */
    void checkInjectionArguments(IRGenerationContext& context) const;
    
    /**
     * Tells whether this is an immediate injection field
     */
    bool isImmediate() const;
    
    /**
     * Frees the object pointed to by the injected field
     */
    void free(IRGenerationContext& context,
              llvm::Value* fieldValue,
              llvm::Value* exception,
              int line) const;

    std::string getName() const override;
    
    const IType* getType() const override;
    
    llvm::Value* getValue(IRGenerationContext& context,
                          const IConcreteObjectType* object,
                          llvm::Value* fieldPointer,
                          int line) const override;

    int getLine() const override;
    
    bool isAssignable(const IConcreteObjectType* object) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
    std::string getFieldKind() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* callInjectFunction(IRGenerationContext& context,
                                    const IConcreteObjectType* controller,
                                    llvm::Value* fieldPointer,
                                    int line) const;

    static void composeInjectFunctionBody(IRGenerationContext& context,
                                          llvm::Function* function,
                                          const void* object1,
                                          const void* object2);

    void checkInterfaceType(IRGenerationContext& context, const Interface* interface) const;
    
    void checkControllerType(IRGenerationContext& context, const Controller* controller) const;
    
  };
  
} /* namespace wisey */

#endif /* InjectedField_h */

