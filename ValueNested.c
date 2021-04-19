#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Value {
  double data;
  double grad;
  struct Backward* backward;
  struct LinkedList* _prev;
  char op[];
} Value;

typedef struct Backward{
  struct Value* self;
  struct Value* other;
  struct Value* out;
  void (*invoke)(struct Value* self, Value* other, Value* out); 
} Backward;

typedef struct Node {
  struct Value* value;
  struct Node* prev;
  struct Node* next;
} Node;

/* Needed for backward (visited = set()) */
typedef struct LinkedList {
  struct Node* head;
  //  void (*append)(struct Node** head, Value* new_value);
} LinkedList;

struct LinkedList* newLinkedList();
struct Backward* newBackward();
struct Value* newValue(double data);
Value* add(Value* self, Value* other);
Value* mul(Value* self, Value* other);
Value* relu(Value* self);
void add_backward(Value* self, Value* other, Value* out);
void mul_backward(Value* self, Value* other, Value* out);
void power_backward(Value* self, Value* other, Value* out);
void relu_backward(Value* self, Value* other, Value* out);
Value* neg(Value* self);
Value* sub(Value* self, Value* other);
Value* truediv(Value* self, Value* other);

void freeLinkedList(struct LinkedList** head_ref);
void freeNodeRec(Node** node);
void freeNode(struct Node** node);
void freeValue(struct Value** Value);
void freeBackward(struct Backward** backward);

Node* wrapValue(Value* value){
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void append(struct Node** head, Value* new_value) {
  Node* new_node = wrapValue(new_value);

  if ((*head) == NULL) {
    new_node->prev = NULL;
    *head = new_node;
    return;
  }
  
  Node* temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;
  
  new_node->prev = temp;
}

/* Check whether p_value is in linked list */
bool search(Node* head, Value* p_value) {
  struct Node* current = head; // Make a node 'current' to search through linked list
  while (current != NULL){
    if (current->value == p_value)
        return true;
    current = current->next;
  }
  return false;
}

struct Value* newValue(double data){
  struct Value* val = (struct Value*)malloc(sizeof(struct Value));

  val->data = data;
  val->_prev = newLinkedList();
  val->grad = 0;
  val->backward = newBackward();

  return val;
}

struct LinkedList* newLinkedList(){
  struct LinkedList* list = (struct LinkedList*)malloc(sizeof(struct LinkedList));
  list->head = NULL;

  return list;
}

struct Backward* newBackward(){
  struct Backward* backward = (struct Backward*)malloc(sizeof(struct Backward));
  backward->self = NULL;
  backward->other = NULL;
  backward->out = NULL;
  backward->invoke = NULL;
}

Value* add (Value* self, Value* other) {
  struct Value* out = newValue(self->data + other->data);

  append(&out->_prev->head, self);
  append(&out->_prev->head, other);

  out->backward->self = self;
  out->backward->other = other;
  out->backward->out = out;
  out->backward->invoke = &add_backward;

  return out;
}

void add_backward(Value* self, Value* other, Value* out){
    self->grad = self->grad + out->grad;
    other->grad = other->grad + out->grad;
}

Value* mul (Value* self, Value* other) {
  struct Value* out = newValue(self->data * other->data);

  append(&out->_prev->head, self);
  append(&out->_prev->head, other);

  out->backward->self = self;
  out->backward->other = other;
  out->backward->out = out;
  out->backward->invoke = &mul_backward;

  return out;
}

void mul_backward(Value* self, Value* other, Value* out){
    self->grad = self->grad + other->data * out->grad;
    other->grad = other->grad + self->data * out->grad;
}

Value* power(Value* self, Value* other){
  struct Value* out = newValue(pow(self->data, other->data));

  append(&out->_prev->head, self);

  out->backward->self = self;
  out->backward->other = other;
  out->backward->out = out;
  out->backward->invoke = &power_backward;
  
  return out;
}

void power_backward(Value* self, Value* other, Value* out){
    self->grad = self->grad + (other->data * pow(self->data, other->data -1.0)) * out->grad;
}

Value* relu(Value* self){
  Value* out = newValue(self->data < 0 ? 0 : self->data);
  append(&out->_prev->head, self);
  
  out->backward->self = self;
  out->backward->out = out;
  out->backward->invoke = &relu_backward;

  return out;
}

void relu_backward(Value* self, Value* other, Value* out){
  self->grad = self->grad + (out->data > 0) * out->grad;
}

void backward(Value* self) {
  // topological order all of the children in the graph
  struct LinkedList* topo = newLinkedList();
  struct LinkedList* visited = newLinkedList();

  void build_topo(Value* self){
      if(!search(visited->head, self)){
          append(&visited->head, self);
          
          Node* childrenPointer = (self->_prev)->head;
          
          while(childrenPointer != NULL){
              build_topo(childrenPointer->value);
              childrenPointer = childrenPointer->next;
          }
          append(&topo->head, self);
      }
  }
  build_topo(self);
  
  // go one variable at a time and apply the chain rule to get its gradient
  self->grad = 1.0;
  if(topo->head != NULL){
    Node* pointer = topo->head;
    while(pointer->next != NULL){
        pointer = pointer->next;
    }


    while(pointer->prev != NULL){
        if(pointer->value->backward->invoke != NULL){
          pointer->value->backward->invoke(
            pointer->value->backward->self,
            pointer->value->backward->other,
            pointer->value->backward->out
          ); // v._backward()
        }
        
        pointer = pointer->prev;
    }
  }
  printf("topo pointer = %p\n", topo);
  freeLinkedList(&topo);
  printf("topo pointer = %p\n", topo);
  freeLinkedList(&visited);
}

int main(){
  Value* x1 = newValue(0);
  Value* y1 = newValue(0);

  Value* x2 = newValue(1);
  Value* y2 = newValue(1);

  Value* x3 = newValue(2);
  Value* y3 = newValue(2);

  Value* x4 = newValue(3);
  Value* y4 = newValue(3);

  Value* x5 = newValue(4);
  Value* y5 = newValue(4);

  Value* a = newValue(-1);
  Value* b = newValue(2);

  Value* step_size = newValue(0.1);

  for(int i = 0; i < 100; i++){

      Value* aCopy = a;
      Value* bCopy = b;

      // d1 = a * x1 + b - y1
      Value* d1 = add(add(mul(aCopy, x1), bCopy), neg(y1));
      Value* d2 = add(add(mul(aCopy, x2), bCopy), neg(y2));
      Value* d3 = add(add(mul(aCopy, x3), bCopy), neg(y3));
      Value* d4 = add(add(mul(aCopy, x4), bCopy), neg(y4));
      Value* d5 = add(add(mul(aCopy, x5), bCopy), neg(y5));
      
      //loss = (d1**2 + d2**2 + d3**2 + d4**2 + d5**2)/5
      Value* lossl = newValue(5.0);
      Value* loss = truediv(
        add(
          add(
            add(
              add(
                power(d1, newValue(2)),
                power(d2, newValue(2))
              ),
              power(d3, newValue(2))
            ),
            power(d4, newValue(2))
          ),
          power(d5, newValue(2))
        ),
        //newValue(5.0)
        lossl
      );

      i = 1 - -1;
      printf("i = %d\n", i);
      
      printf("loss last newValue pointer = %p\n", lossl);

      backward(loss);
      //copy a and b
      //put data of copy of a and b into original a and b
      //free loss

      aCopy = sub(aCopy, mul(newValue(aCopy->grad), step_size));
      bCopy = sub(bCopy, mul(newValue(bCopy->grad), step_size));

      a = aCopy;
      b = bCopy;

      //a->_prev->head = NULL;
      //b->_prev->head = NULL;

      // a->grad = 0;
      // b->grad = 0;
      struct Node* current = loss->_prev->head;
      struct Node* next;
      while (current != NULL){
        next = current->next;
        printf("loss children = %p \n", current);
        current = next;
      }
      //freeloss(loss);
  }
  printf("a data after: %0.20lf a grad after %0.20lf\n", a->data, a->grad);
  printf("b data after: %0.20lf b grad after %0.20lf\n", b->data, b->grad);

  return 0;
}

Value* neg(Value* self){
  return mul(self, newValue(-1.0));
}

Value* sub(Value* self, Value* other){
  return add(self, neg(other));
}

Value* truediv(Value* self, Value* other){
  return mul(self, power(other, newValue(-1.0)));
}


/* Function to delete linked list */
void freeLinkedList(struct LinkedList** linkedList){
  struct Node* current = (*linkedList)->head;
  struct Node* next;

  freeNodeRec(&(*linkedList)->head);
  // while (current != NULL)
  // {
  //   next = current->next;
  //   //printf("current data = %lf\n", current->value->data);
  //   free(current);
  //   //current = NULL;
  //   //printf("current data = %lf\n", current->value->data);
  //   current = next;
  // }
  free(*linkedList);
  *linkedList = NULL;
}

// 

// typedef struct Value {       <----- malloc
//   double data;
//   double grad;
//   struct Backward* backward; <----- malloc
//   struct LinkedList* _prev;  <----- malloc
//   char op[];
// } Value;


void freeNodeRec(Node** node){
  if((*node) == NULL){
    return;
  } 
  Node* next = (*node)->next;
  freeNodeRec(&next);
  //freeNode(node);
  free(node);
}

void freeNode(struct Node** node){
  freeValue(&((*node)->value));
  free(*node);
  *node = NULL;
}

void freeValue(struct Value** value){
  freeBackward(&((*value)->backward));
  freeLinkedList(&((*value)->_prev));
  free(*value);
  *value = NULL;
}

void freeBackward(struct Backward** backward){
  free(*backward);
  *backward = NULL;
}