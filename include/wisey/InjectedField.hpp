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
    int mLine;
    
  public:
    
    InjectedField(const IType* type,
                  const IType* injectedType,
                  std::string name,
                  InjectionArgumentList injectionArgumentList,
                  std::string sourceFileName,
                  int line);
    
    ~InjectedField();
    
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
    std::string getInjectionFunctionName(const Controller* controller) const;
    
    /**
     * Calls the function that injects the field value and stores it at the provided location
     */
    llvm::Value* callInjectFunction(IRGenerationContext& context,
                                    const Controller* controller,
                                    llvm::Value* fieldPointer,
                                    int line) const;
    
    /**
     * Checks that field injected type is of object owner or array owner type
     */
    void checkType(IRGenerationContext& context) const;
    
    /**
     * Checks that injection arguments are all there and match received types
     */
    void checkInjectionArguments(IRGenerationContext& context) const;

    std::string getName() const override;
    
    const IType* getType() const override;
    
    int getLine() const override;
    
    bool isAssignable() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

    bool isFixed() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:

    static void composeInjectFunctionBody(IRGenerationContext& context,
                                          llvm::Function* function,
                                          const void* object1,
                                          const void* object2);

    void checkInterfaceType(IRGenerationContext& context, const Interface* interface) const;
    
    void checkControllerType(IRGenerationContext& context, const Controller* controller) const;
    
  };
  
} /* namespace wisey */

#endif /* InjectedField_h */

