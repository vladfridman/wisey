#include <map>

/**
 * Library of functions that work with STL data structures
 */

/**
 * Creates a hash map
 */
extern "C" void* stl_map_create() {
  return new std::map<void*, void*>();
}

/**
 * Destroys a hash map
 */
extern "C" void stl_map_destroy(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  delete mapCast;
}

/**
 * Puts a key value pair into a hash map
 */
extern "C" void stl_map_put(void* map, void* key, void* value) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  mapCast->insert(std::pair<void*, void*>(key, value));
}

/**
 * Return a value for the given key from a hash map
 */
extern "C" void* stl_map_get(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (mapCast->count(key)) {
    return mapCast->at(key);
  }
  return NULL;
}

/**
 * Decrement references of objects contained in the map
 */
extern "C" void stl_map_decrement_reference_counts(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    int8_t** object = (int8_t**) iterator->second;
    int8_t* firstTable = *object;
    int64_t* offsetPointer = (int64_t*) firstTable;
    int64_t offset = *offsetPointer;
    int8_t* objectStart = ((int8_t*) object) - offset;
    int8_t* objectShell = objectStart - sizeof(int64_t);
    int64_t* referenceCounter = (int64_t*) objectShell;
    *referenceCounter = *referenceCounter - 1;
  }
}

/**
 * Decrement references of objects contained in the map
 */
extern "C" void stl_map_destroy_objects(void* map, void (*destructor) (int8_t*)) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    int8_t* object = (int8_t*) iterator->second;
    destructor(object);
  }
}
