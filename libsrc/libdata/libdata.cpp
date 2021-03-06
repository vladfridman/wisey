#include <algorithm>
#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <thread>
#include <unordered_map>
#include <vector>

FILE* wStdIn = stdin;
FILE* wStdOut = stdout;
FILE* wStdErr = stderr;
int64_t wProcessorCount = std::thread::hardware_concurrency();

/**
 * Library of functions that work with STL data structures and memory pool
 */

void adjust_wisey_object_reference_count(void* objectPointer, int adjustment);
void destroy_wisey_object(void* objectPointer);

/**
 * Function to support lists where value is an object reference or owner reference
 */
extern "C" void* stl_object_list_create() {
  return new std::list<void*>();
}

extern "C" void* stl_reference_list_pop_back(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  void* back = list->back();
  list->pop_back();
  adjust_wisey_object_reference_count(back, -1);
  return back;
}

extern "C" void* stl_owner_list_pop_back(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  void* back = list->back();
  list->pop_back();
  return back;
}

extern "C" void stl_reference_list_push_back(void* listUncast, void* object) {
  auto list = (std::list<void*>*) listUncast;
  adjust_wisey_object_reference_count(object, 1);
  list->push_back(object);
}

extern "C" void stl_owner_list_push_back(void* listUncast, void* object) {
  auto list = (std::list<void*>*) listUncast;
  list->push_back(object);
}

extern "C" void* stl_object_list_back(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  return list->back();
}

extern "C" void* stl_reference_list_pop_front(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  void* front = list->front();
  list->pop_front();
  adjust_wisey_object_reference_count(front, -1);
  return front;
}

extern "C" void* stl_owner_list_pop_front(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  void* front = list->front();
  list->pop_front();
  return front;
}

extern "C" void* stl_object_list_front(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  if (!list->size()) {
    return NULL;
  }
  return list->front();
}

extern "C" void stl_reference_list_push_front(void* listUncast, void* object) {
  auto list = (std::list<void*>*) listUncast;
  adjust_wisey_object_reference_count(object, 1);
  list->push_front(object);
}

extern "C" void stl_owner_list_push_front(void* listUncast, void* object) {
  auto list = (std::list<void*>*) listUncast;
  list->push_front(object);
}

extern "C" int64_t stl_object_list_size(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  return list->size();
}

extern "C" void stl_reference_list_clear(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  for (void* object : *list) {
    adjust_wisey_object_reference_count(object, -1);
  }
  list->clear();
}

extern "C" void stl_owner_list_clear(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  for (void* object : *list) {
    destroy_wisey_object(object);
  }
  list->clear();
}

extern "C" void stl_object_list_destroy(void* listUncast) {
  auto list = (std::list<void*>*) listUncast;
  delete list;
}

/**
 * Function to support hashmaps where key is an object reference and value is an object reference
 */
extern "C" void* stl_reference_to_object_map_create() {
  return new std::unordered_map<void*, void*>();
}

extern "C" void stl_reference_to_object_map_destroy(void* map) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  delete mapCast;
}

extern "C" void stl_reference_to_reference_map_erase(void* map, void* key) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = (*mapCast)[key];
  adjust_wisey_object_reference_count(value, -1);
  mapCast->erase(key);
}

extern "C" void stl_reference_to_reference_map_put(void* map, void* key, void* value) {
  stl_reference_to_reference_map_erase(map, key);
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  (*mapCast)[key] = value;
  adjust_wisey_object_reference_count(value, 1);
}

extern "C" void* stl_reference_to_object_map_get(void* map, void* key) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  if (mapCast->count(key)) {
    return (*mapCast)[key];
  }
  return NULL;
}

extern "C" void stl_reference_to_reference_map_clear(void* map) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  for (auto iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    adjust_wisey_object_reference_count(iterator->second, -1);
  }
  mapCast->clear();
}

extern "C" int64_t stl_reference_to_object_map_size(void* map) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  return mapCast->size();
}

/**
 * Function to support hashmaps where key is an object reference and value is an object owner
 */
extern "C" void stl_reference_to_owner_map_erase(void* map, void* key) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = (*mapCast)[key];
  destroy_wisey_object(value);
  mapCast->erase(key);
}

extern "C" void stl_reference_to_owner_map_put(void* map, void* key, void* value) {
  stl_reference_to_owner_map_erase(map, key);
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  (*mapCast)[key] = value;
}

extern "C" void* stl_reference_to_owner_map_take(void* map, void* key) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return NULL;
  }
  void* result = (*mapCast)[key];
  mapCast->erase(key);
  return result;
}

extern "C" void stl_reference_to_owner_map_clear(void* map) {
  auto mapCast = (std::unordered_map<void*, void*>*) map;
  for (auto iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    destroy_wisey_object(iterator->second);
  }
  mapCast->clear();
}

/**
 * Function to support hashmaps where key is of type long and value is of type int
 */
extern "C" void* stl_long_to_int_map_create() {
  auto map = new std::unordered_map<int64_t, int32_t>();
  return map;
}

extern "C" void stl_long_to_int_map_destroy(void* map) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  delete mapCast;
}

extern "C" void stl_long_to_int_map_erase(void* map, int64_t key) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  mapCast->erase(key);
}

extern "C" void stl_long_to_int_map_put(void* map, int64_t key, int32_t value) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  (*mapCast)[key] = value;
}

extern "C" bool stl_long_to_int_map_has(void* map, int64_t key) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  return mapCast->count(key);
}

extern "C" void stl_long_to_int_map_addTo(void* map, int64_t key, int32_t value) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  (*mapCast)[key] += value;
}

extern "C" int32_t stl_long_to_int_map_get(void* map, int64_t key) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  return (*mapCast)[key];
}

extern "C" void stl_long_to_int_map_clear(void* map) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  mapCast->clear();
}

extern "C" int64_t stl_long_to_int_map_size(void* map) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  return mapCast->size();
}

/**
 * Returns a wisey array containing values of a long to int map
 */
extern "C" void* stl_long_to_int_map_get_values_array(void *map) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  int64_t allocSize = mapCast->size() * sizeof(int32_t) + 3 * sizeof(int64_t);
  void* memory = malloc(allocSize);
  int8_t* location = (int8_t*) memory;
  int64_t* referenceCounter = (int64_t*) location;
  *referenceCounter = 0;
  location += sizeof(int64_t);
  int64_t* arraySize = (int64_t*) location;
  *arraySize = mapCast->size();
  location += sizeof(int64_t);
  int64_t* elementSize = (int64_t*) location;
  *elementSize = sizeof(int32_t);
  location += sizeof(int64_t);
  int32_t* element = (int32_t*) location;

  for (auto iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    *element = iterator->second;
    element++;
  }

  return memory;
}

/**
 * Returns a wisey array containing keys of a long to int map
 */
extern "C" void* stl_long_to_int_map_get_keys_array(void *map) {
  auto mapCast = (std::unordered_map<int64_t, int32_t>*) map;
  int64_t allocSize = mapCast->size() * sizeof(int64_t) + 3 * sizeof(int64_t);
  void* memory = malloc(allocSize);
  int8_t* location = (int8_t*) memory;
  int64_t* referenceCounter = (int64_t*) location;
  *referenceCounter = 0;
  location += sizeof(int64_t);
  int64_t* arraySize = (int64_t*) location;
  *arraySize = mapCast->size();
  location += sizeof(int64_t);
  int64_t* elementSize = (int64_t*) location;
  *elementSize = sizeof(int64_t);
  location += sizeof(int64_t);
  int64_t* element = (int64_t*) location;

  for (auto iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    *element = iterator->first;
    element++;
  }

  return memory;
}

/**
 * Destroys a wisey object by finding a destructor method in its vtable and calling it
 */
void destroy_wisey_object(void* objectPointer) {
  int8_t** object = (int8_t**) objectPointer;
  int8_t* vTablePortion = *object;
  int64_t* offsetPointer = (int64_t*) vTablePortion;
  int64_t offset = *offsetPointer;
  int8_t* objectStart = ((int8_t*) object) - offset;
  int8_t*** vTable = (int8_t***) objectStart;
  int8_t** firstTable = *vTable;
  int8_t* destructorPointer = firstTable[2];
  void (*destructor) (int8_t*, int8_t*, int8_t*, int8_t*) =
     (void (*) (int8_t*, int8_t*, int8_t*, int8_t*)) destructorPointer;

  destructor(objectStart, NULL, NULL, NULL);
}

/**
 * Adjust reference count of a wisey object
 */
void adjust_wisey_object_reference_count(void* objectPointer, int adjustment) {
  int8_t** object = (int8_t**) objectPointer;
  int8_t* vTablePortion = *object;
  int64_t* offsetPointer = (int64_t*) vTablePortion;
  int64_t offset = *offsetPointer;
  int8_t* objectStart = ((int8_t*) object) - offset;
  int8_t* objectShell = objectStart - sizeof(int64_t);
  int64_t* referenceCounter = (int64_t*) objectShell;

  int8_t*** vTable = (int8_t***) objectStart;
  int8_t** firstTable = *vTable;
  int8_t** typeTable = (int8_t**) firstTable[1];
  char* typeShortName = (char*) typeTable[0];
  char firstLetter = typeShortName[0];
  if (firstLetter == 'm' || firstLetter == 't') {
    __sync_fetch_and_add(referenceCounter, adjustment);
  } else {
    *referenceCounter = *referenceCounter + adjustment;
  }
}

/**
 * Functions to support wisey vector of owner references
 */
extern "C" void* stl_object_vector_create() {
  return new std::vector<void*>();
}

extern "C" void stl_owner_vector_clear(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  for (std::vector<void*>::iterator iterator = vectorCast->begin(); iterator != vectorCast->end(); iterator++) {
    destroy_wisey_object(*iterator);
  }
  vectorCast->clear();
}

extern "C" void stl_object_vector_destroy(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  delete vectorCast;
}

extern "C" void stl_owner_vector_push_back(void* vector, void* object) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  vectorCast->push_back(object);
}

extern "C" void* stl_owner_vector_pop_back(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  void* last = vectorCast->back();
  vectorCast->pop_back();
  return last;
}

extern "C" void* stl_object_vector_at(void* vector, int64_t index) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  return vectorCast->at(index);
}

extern "C" int64_t stl_object_vector_size(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  return vectorCast->size();
}

extern "C" void stl_object_vector_sort(void* vector, 
                                       void* object,
                                       void* thread,
                                       void* callstack) {
  int8_t*** vTable = (int8_t***) object;
  int8_t** firstTable = *vTable;
  int8_t* functionPointer = firstTable[3];
  bool (*compare_function) (void*, void*, void*, void*, void*) =
     (bool (*) (void*, void*, void*, void*, void*)) functionPointer;

  auto vectorCast = (std::vector<void*>*) vector;
  std::sort(vectorCast->begin(), vectorCast->end(), [&compare_function, &object, &thread, &callstack](void* a, void* b) {
    return compare_function(object, thread, callstack, a, b);   
  });
}

extern "C" void stl_object_list_sort(void* list, 
                                     void* object,
                                     void* thread,
                                     void* callstack) {
  int8_t*** vTable = (int8_t***) object;
  int8_t** firstTable = *vTable;
  int8_t* functionPointer = firstTable[3];
  bool (*compare_function) (void*, void*, void*, void*, void*) =
     (bool (*) (void*, void*, void*, void*, void*)) functionPointer;

  auto listCast = (std::list<void*>*) list;
  listCast->sort([&compare_function, &object, &thread, &callstack](void* a, void* b) {
    return compare_function(object, thread, callstack, a, b);   
  });
}

/**
 * Functions to support wisey vector of object references
 */
extern "C" void stl_reference_vector_clear(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  for (std::vector<void*>::iterator iterator = vectorCast->begin(); iterator != vectorCast->end(); iterator++) {
    adjust_wisey_object_reference_count(*iterator, -1);
  }
  vectorCast->clear();
}

extern "C" void stl_reference_vector_push_back(void* vector, void* object) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  adjust_wisey_object_reference_count(object, 1);
  vectorCast->push_back(object);
}

extern "C" void* stl_reference_vector_pop_back(void* vector) {
  std::vector<void*>* vectorCast = (std::vector<void*>*) vector;
  void* last = vectorCast->back();
  vectorCast->pop_back();
  adjust_wisey_object_reference_count(last, -1);
  return last;
}

#define MEM_POOL_START_SIZE 1024 * 1024
#define LONG_TO_POINTER(i) ((int64_t*)(i))
#define POINTER_TO_LONG(p) ((long)(p))

/**
 * Memory pool implementation
 * 
 * Memory pool object contains
 * ---------
 * i64* - pointer to the free memory slot
 * i64* - pointer to the end of the current block
 * i64* - pointer to the current block
 * --------- start of the first block which is part of memory pool object itself
 * i64* - next memory pool block
 * i64 this block size in bytes
 * ... - memory for allocation
 * --------- 
 * 
 * Additional memory blocks look as follows
 * ---------
 * i64* - next memory pool block
 * i64 this block size in bytes
 * ... - memory for allocation
 * --------- 
 */
extern "C" void* mem_pool_create() {
  int64_t* pool = (int64_t*) malloc(MEM_POOL_START_SIZE);
  pool[0] = POINTER_TO_LONG(pool) + sizeof(int64_t) * 5;
  pool[1] = POINTER_TO_LONG(pool) + MEM_POOL_START_SIZE;
  pool[2] = POINTER_TO_LONG(pool) + sizeof(int64_t) * 3;
  pool[3] = 0;
  pool[4] = MEM_POOL_START_SIZE - sizeof(int64_t) * 5;
  return pool;
}

void* mem_pool_alloc_cont(int64_t* pool, int64_t size);

/**
 * Allocate an object of given size on the given memory pool
 */
extern "C" void* mem_pool_alloc(void* memory_pool, int64_t size) {
  int64_t* pool = (int64_t*) memory_pool;
  int64_t next_space_address = pool[0];
  if (next_space_address + size <= pool[1]) {
    pool[0] += size;
    return LONG_TO_POINTER(next_space_address);
  }
  return mem_pool_alloc_cont(pool, size);
}

/**
 * Allocates an object of the given size on the given memory pool in a reused or a new block
 */
void* mem_pool_alloc_cont(int64_t* pool, int64_t size) {
  int64_t current_block_address = pool[2];
  int64_t* current_block = LONG_TO_POINTER(current_block_address);
  int64_t block_address = current_block[0];

  while (block_address != 0) {
    int64_t* block = LONG_TO_POINTER(block_address);
    pool[0] = block_address + sizeof(int64_t) * 2;
    pool[1] = pool[0] + block[1];
    pool[2] = block_address;
    int64_t next_space_address = pool[0];
    if (next_space_address + size <= pool[1]) {
      pool[0] += size;
      return LONG_TO_POINTER(next_space_address);
    }
    block_address = block[0];
  }

  current_block_address = pool[2];
  current_block = LONG_TO_POINTER(current_block_address);
  int64_t current_block_size = current_block[1];
  int64_t min_size = size + sizeof(int64_t) * 2;
  int64_t next_size = current_block_size * 4;
  int64_t new_block_size = next_size > min_size ? next_size : min_size;

  int64_t* new_block = (int64_t*) malloc(new_block_size);
  int64_t new_block_address = POINTER_TO_LONG(new_block);
  current_block[0] = POINTER_TO_LONG(new_block);
  new_block[0] = 0;
  new_block[1] = new_block_size - 2 * sizeof(int64_t);
  pool[0] = new_block_address + 2 * sizeof(int64_t) + size;
  pool[1] = new_block_address + new_block_size;
  pool[2] = new_block_address;
  return &(new_block[2]);
}

/**
 * Clears the memory pool without destroying any of the blocks
 */
extern "C" void mem_pool_clear(void* memory_pool) {
  int64_t* pool = (int64_t*) memory_pool;
  pool[0] = POINTER_TO_LONG(pool) + sizeof(int64_t) * 5;
  pool[1] = POINTER_TO_LONG(pool) + MEM_POOL_START_SIZE;
  pool[2] = POINTER_TO_LONG(pool) + sizeof(int64_t) * 3;
}

/**
 * Destroys the memory pool and all allocated blocks
 */
extern "C" void mem_pool_destroy(void* memory_pool) {
  int64_t* pool = (int64_t*) memory_pool;
  int64_t block_address = pool[3];
  while (block_address != 0) {
    int64_t* block = LONG_TO_POINTER(block_address);
    block_address = block[0];
    free(block);
  }
  free(memory_pool);
}

/**
 * Simple concurrent queue implementation
 * 
 * The concurrent queue is used for thread pool implementation, it is used to store model owners
 */
class ConcurrentQueue {
  std::queue<void*> queue;
  std::mutex mutex;
  std::condition_variable cond;

public:
 
  void push(void* item) {
    std::unique_lock<std::mutex> mlock(mutex);
    queue.push(item);
    mlock.unlock();
    cond.notify_one();
  }
 
  void* pop() {
    std::unique_lock<std::mutex> mlock(mutex);
    while (queue.empty()) {
      cond.wait(mlock);
    }
    auto item = queue.front();
    queue.pop();
    return item;
  }
};

/**
 * Api functions to operate the concurrent queue
 */
extern "C" void* concurrent_model_owner_queue_create() {
  return new ConcurrentQueue();
}

extern "C" void concurrent_model_owner_queue_destroy(void* queue) {
  ConcurrentQueue* queueCast = (ConcurrentQueue*) queue;
  delete queueCast;
}

extern "C" void concurrent_model_owner_queue_push(void* queue, void* item) {
  ConcurrentQueue* queueCast = (ConcurrentQueue*) queue;
  queueCast->push(item);
}

extern "C" void* concurrent_model_owner_queue_pop(void* queue) {
  ConcurrentQueue* queueCast = (ConcurrentQueue*) queue;
  return queueCast->pop();
}

/**
 * Returns a random number in the given range
 */
extern "C" int get_random_number(int from, int to) {
  int range = to - from + 1;
  return rand() % range + from;
}
