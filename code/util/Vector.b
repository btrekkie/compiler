class Vector {
  Int[] elements = new Int[4];
  Int size = 0;
  
  void copy(Int[] dest, Int[] source, Int numElements) {
    for (Int i = 0; i < numElements; i++) {
      dest[i] = source[i];
    }
  }
  
  Int get(Int index) {
    return elements[index];
  }
  
  Int getSize() {
    return size;
  }

  void add(Int value) {
    if (size == elements.length) {
      Int[] newElements = new Int[size * 2];
      copy(newElements, elements, size);
      elements = newElements;
    }
    elements[size] = value;
    size++;
  }

  Int pop() {
    size--;
    return elements[size];
  }
  
  Int[] toArray() {
    Int[] result = new Int[size];
    copy(result, elements, size);
    return result;
  }
  
  Int hash() {
    Int hash = 1;
    for (Int i = 0; i < size; i++)
      hash = 31 * hash + elements[i];
    return hash;
  }
}
