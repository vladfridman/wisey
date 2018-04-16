//
//  IInjectableConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IInjectableConcreteObjectType_h
#define IInjectableConcreteObjectType_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {

  /**
   * Represents an injectable concrete object such as a controller or a thread
   */
  class IInjectableConcreteObjectType : public IConcreteObjectType {
    
  public:
    
    /**
     * Inject an instance of this injectable object with given arguments
     */
    virtual llvm::Value* inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const = 0;
    
    /**
     * Returns the list of recieved fields that is not in the list of given fields
     */
    virtual std::vector<std::string> getMissingReceivedFields(std::set<std::string>
                                                              givenFields) const = 0;
    
    /**
     * Returns the list of fields the object is injecting
     */
    virtual std::vector<InjectedField*> getInjectedFields() const = 0;

    static llvm::Instruction* injectObject(IRGenerationContext& context,
                                           const IInjectableConcreteObjectType* object,
                                           const InjectionArgumentList injectionArgumentList,
                                           int line);
    
  private:
    
    static void checkArguments(const IInjectableConcreteObjectType* object,
                               const InjectionArgumentList& injectionArgumentList);
    
    static void checkArgumentsAreWellFormed(const IInjectableConcreteObjectType* object,
                                            const InjectionArgumentList& injectionArgumentList);
    
    static void checkAllFieldsAreSet(const IInjectableConcreteObjectType* object,
                                     const InjectionArgumentList& injectionArgumentList);
    
    static void initializeReceivedFields(IRGenerationContext& context,
                                         const IInjectableConcreteObjectType* object,
                                         const InjectionArgumentList& controllerInjectorArguments,
                                         llvm::Instruction* malloc,
                                         int line);
    
    static void initializeInjectedFields(IRGenerationContext& context,
                                         const IInjectableConcreteObjectType* object,
                                         llvm::Instruction* malloc);

  };
  
} /* namespace wisey */

#endif /* IInjectableConcreteObjectType_h */
