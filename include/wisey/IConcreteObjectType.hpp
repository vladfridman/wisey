//
//  IConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectType_h
#define IConcreteObjectType_h

#include <llvm/IR/Instructions.h>

#include "wisey/Constant.hpp"
#include "wisey/IField.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
  class Interface;
  class LLVMFunction;
  
  /**
   * This represents a concrete object which every type of object except interface
   *
   * All objects have vTables. The pointer pointing to an object points to its vTable.
   * The object's reference counter is stored 8 bytes before the address of the object.
   * Reference counter is 64 bit integer and it occupies the whole 8 bytes.
   *
   * vTable is an array of arrays of i8* pointers:
   * [
   *   [vTable portion for the concrete object and first implemented interface],
   *   [vTable portion for the second implemented interface],
   *   [vTable portion for the third implemented interface],
   *   ...
   * ]
   *
   * vTable portion for the concrete object and first implemented interface consists of
   * [
   *   i8* null,
   *   i8* <pointer to type table array>,
   *   i8* <pointer to destructor>,
   *   i8* <pointer to first interface method that is a concrete object's method>,
   *   i8* <pointer to second interface method that is a concrete object's method>,
   *   ...
   * ]
   *
   * vTable portion for an implemented interface consists of
   * [
   *   i8* <number of bytes needed to add to the interface object pointer to get to concrete object>
   *   i8* null,
   *   i8* null,
   *   i8* <pointer to first interface method that maps to concrete object method>,
   *   i8* <pointer to second interface method that maps to concrete object method>,
   *   ...
   * ]
   *
   * Typetable for the concrete object consists of array of pointers to type names:
   * [
   *   i8* <concrete object type short name>,
   *   i8* <concrete object type full name>,
   *   i8* <first interface type full name>,
   *   i8* <second interface type full name>,
   *   ...,
   *   i8* null
   * ]
   *
   */
  class IConcreteObjectType : public IObjectType {
    
  public:
    
    virtual ~IConcreteObjectType() { }
    
    /**
     * Contains ascii code for the letter 'c'
     */
    static const unsigned int CONTROLLER_FIRST_LETTER_ASCII_CODE;

    /**
     * Contains ascii code for the letter 'm'
     */
    static const unsigned int MODEL_FIRST_LETTER_ASCII_CODE;

    /**
     * Contains ascii code for the letter 'n'
     */
    static const unsigned int NODE_FIRST_LETTER_ASCII_CODE;

    /**
     * Contains ascii code for the letter 't'
     */
    static const unsigned int THREAD_FIRST_LETTER_ASCII_CODE;

    /**
     * Looks for a field with a given name in the object
     */
    virtual IField* findField(std::string fieldName) const = 0;
    
    /**
     * Returns field index in the struct data type representing this object
     */
    virtual unsigned long getFieldIndex(const IField* field) const = 0;
    
    /**
     * Returns a list of all fields
     */
    virtual std::vector<IField*> getFields() const = 0;
    
    /**
     * Returns the name of the vTable global varaible
     */
    virtual std::string getVTableName() const = 0;
    
    /**
     * Returns the size of the VTable array for this object
     */
    virtual unsigned long getVTableSize() const = 0;
    
    /**
     * Returns interfaces that this object implements
     */
    virtual std::vector<Interface*> getInterfaces() const = 0;
    
    /**
     * Returns a list of all interfaces this model implements including inherited interfaces
     */
    virtual std::vector<Interface*> getFlattenedInterfaceHierarchy() const = 0;
    
    /**
     * Returns the name of the global varaible containing types that this model implements
     */
    virtual std::string getTypeTableName() const = 0;
    
    /**
     * Finds a method with a given name.
     */
    virtual const IMethodDescriptor* findMethod(std::string methodName) const = 0;
    
    /**
     * Returns the map of method names to methods
     */
    virtual std::map<std::string, IMethod*> getNameToMethodMap() const = 0;
    
    /**
     * Get list of all methods
     */
    virtual std::vector<IMethod*> getMethods() const = 0;
    
    /**
     * Set fields to the given map of fields
     */
    virtual void setFields(IRGenerationContext& context,
                           std::vector<IField*> fields,
                           unsigned long startIndex) = 0;
    
    /**
     * Set interfaces for this object
     */
    virtual void setInterfaces(std::vector<Interface*> interfaces) = 0;
    
    /**
     * Set methods for this object
     */
    virtual void setMethods(std::vector<IMethod*> methods) = 0;
    
    /**
     * Set body types of the struct that represents this object
     */
    virtual void setStructBodyTypes(std::vector<llvm::Type*> types) = 0;
    
    /**
     * Sets constants defined within this object
     */
    virtual void setConstants(std::vector<Constant*> constants) = 0;
    
    /**
     * Returns constants defined within this object
     */
    virtual std::vector<Constant*> getConstants() const = 0;
    
    /**
     * Sets llvm functions defined within this object
     */
    virtual void setLLVMFunctions(std::vector<LLVMFunction*> constants) = 0;
    
    /**
     * Returns llvm function defined within this object
     */
    virtual std::vector<LLVMFunction*> getLLVMFunctions() const = 0;
    
    /**
     * Returns name of the global variable containing short name of this concrete Object
     */
    virtual std::string getObjectShortNameGlobalVariableName() const = 0;
    
    /**
     * Generates a global with object type name. e.g. model or controller
     */
    virtual llvm::Constant* getObjectTypeNameGlobal(IRGenerationContext& context) const = 0;
    
    /**
     * Generate global variable with the name of the given object
     */
    static void generateNameGlobal(IRGenerationContext& context, const IConcreteObjectType* object);
    
    /**
     * Generate global variable with the short name of the given object
     */
    static void generateShortNameGlobal(IRGenerationContext& context,
                                        const IConcreteObjectType* object);
    
    /**
     * Casts this concrete object to a given type
     */
    static llvm::Value* castTo(IRGenerationContext& context,
                               const IConcreteObjectType* object,
                               llvm::Value* fromValue,
                               const IType* toType,
                               int line);
    
    /**
     * Returns the interface index in the list of flattened interface hierarchy
     */
    static int getInterfaceIndex(const IConcreteObjectType* object,
                                 const Interface* interface);
    
    /**
     * Initialize vTable of a newly created concrete object
     */
    static void initializeVTable(IRGenerationContext& context,
                                 const IConcreteObjectType* object,
                                 llvm::Instruction* malloc);
    
    /**
     * Generate vTable global variable for the given object
     */
    static void generateVTable(IRGenerationContext& context,
                               const IConcreteObjectType* object);
    
    /**
     * Declare all fields as variables in the current scope
     */
    static void declareFieldVariables(IRGenerationContext& context,
                                      const IConcreteObjectType* object);
    
    /**
     * Adds a callback to compose destructor function code
     */
    static void scheduleDestructorBodyComposition(IRGenerationContext& context,
                                                  const IConcreteObjectType* object);
    
    /**
     * Compose a call to destroy a given concrete object
     */
    static void composeDestructorCall(IRGenerationContext& context, llvm::Value* value);
    
    /**
     * Returns destructor function
     */
    static llvm::Function* getDestructorFunctionForObject(IRGenerationContext& context,
                                                          const IConcreteObjectType* object,
                                                          int line);
    
    /**
     * Generates IR for static methods of a given object
     */
    static void generateStaticMethodsIR(IRGenerationContext& context,
                                        const IConcreteObjectType* object);
    
    /**
     * Generates IR for non-static methods of a given object
     */
    static void generateMethodsIR(IRGenerationContext& context, const IConcreteObjectType* object);
    
    /**
     * Generates IR for constants defined in the object
     */
    static void generateConstantsIR(IRGenerationContext& context, const IConcreteObjectType* object);
    
    /**
     * Generates llvm function declarations
     */
    static void declareLLVMFunctions(IRGenerationContext& context,
                                     const IConcreteObjectType* object);
    
    /**
     * Generates llvm functions' bodies
     */
    static void generateLLVMFunctionsIR(IRGenerationContext& context,
                                        const IConcreteObjectType* object);
    
    /**
     * Print the given object to the given stream
     */
    static void printObjectToStream(IRGenerationContext& context,
                                    const IConcreteObjectType* object,
                                    std::iostream& stream);
    
    /**
     * Define variable containing current object name
     */
    static void defineCurrentObjectNameVariable(IRGenerationContext& context,
                                                const IConcreteObjectType* objectType);
    
    /**
     * Add the given interface and all of its parents to the given vector of interfaces
     */
    static void addInterfaceAndItsParents(Interface* interface, std::vector<Interface*>& result);
    
    /**
     * Tells whether one can cast fromType to toType
     */
    static bool canCast(const IType* fromType, const IType* toType);
    
    /**
     * Compose map functions IR. This is not called for externally defined objects
     */
    static void composeInterfaceMapFunctions(IRGenerationContext& context,
                                             const IConcreteObjectType* object);
    
    /**
     * Returns destructor function type that is the same for all objects
     */
    static llvm::FunctionType* getDestructorFunctionType(IRGenerationContext& context);
    
    /**
     * Returns the size of the VTable array for the given object
     */
    static unsigned long getVTableSizeForObject(const IConcreteObjectType* object);

    /**
     * Allocate memory for a new object of the given type
     */
    static llvm::Instruction* createMallocForObject(IRGenerationContext& context,
                                                    const IConcreteObjectType* object,
                                                    std::string variableName);
    
    /**
     * Searches list of methods in the given objects and in its inherented interfaces
     */
    static const IMethod* findMethodInObject(std::string methodName,
                                             const IConcreteObjectType* object);
    
  private:
    
    static std::map<std::string, llvm::Function*>
    defineMethodFunctions(IRGenerationContext& context, const IConcreteObjectType* object);
    
    static void addTypeListInfo(IRGenerationContext& context,
                                const IConcreteObjectType* object,
                                std::vector<std::vector<llvm::Constant*>>& vTables);
    
    static void addUnthunkInfo(IRGenerationContext& context,
                               const IConcreteObjectType* object,
                               std::vector<std::vector<llvm::Constant*>>& vTables);
    
    static void declareInterfaceMapFunctions(IRGenerationContext& context,
                                             const IConcreteObjectType* object,
                                             std::vector<std::vector<llvm::Constant*>>& vTables);
    
    static void defineVTableGlobal(IRGenerationContext& context,
                                   const IConcreteObjectType* object,
                                   std::vector<std::vector<llvm::Constant*>> interfaceVTables);

    static void declareVTableGlobal(IRGenerationContext& context,
                                    const IConcreteObjectType* object,
                                    std::vector<std::vector<llvm::Constant*>> interfaceVTables);

    static llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context,
                                                      const IConcreteObjectType* object);
    
    static void addDestructorInfo(IRGenerationContext& context,
                                  const IConcreteObjectType* objet,
                                  std::vector<std::vector<llvm::Constant*>>& vTables);
    
    static std::string getObjectDestructorFunctionName(const IConcreteObjectType* object);
    
    static void decrementReferenceFields(IRGenerationContext& context,
                                         llvm::Value* thisValue,
                                         const IConcreteObjectType* object);
    
    static void freeOwnerFields(IRGenerationContext& context,
                                llvm::Value* thisValue,
                                const IConcreteObjectType* object,
                                int line);
    
    static llvm::Value* getFieldValuePointer(IRGenerationContext& context,
                                             llvm::Value* thisValue,
                                             const IConcreteObjectType* object,
                                             IField* field);
    
    static llvm::Value* getFieldPointer(IRGenerationContext& context,
                                        llvm::Value* thisValue,
                                        const IConcreteObjectType* object,
                                        IField* field);
    
    static void composeDestructorBody(IRGenerationContext& context,
                                      llvm::Function* function,
                                      const void* object);
    
    static void printTypeKind(const IConcreteObjectType* type, std::iostream& stream);
    
    static llvm::StructType* getOrCreateRefCounterStruct(IRGenerationContext& context,
                                                         const IConcreteObjectType* object);
    
    static void checkMethodOverride(IRGenerationContext& context,
                                    const IConcreteObjectType* object,
                                    IMethod* method);
  };
  
} /* namespace wisey */

#endif /* IConcreteObjectType_h */

