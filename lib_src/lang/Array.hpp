#ifndef COMPILER_Array_DEFINED
#define COMPILER_Array_DEFINED

class c_Array {
public:
    void* f_elements;
    int f_length;
    void constructor(void* elements, int length);
};

#endif
