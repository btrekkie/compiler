class Array<T> {
  public native T[] subarray(int startIndex);
  public native T[] subarray(int startIndex, int endIndex);
  public native T[] getCopy();
  public native void copyTo(T[] buffer);
  public native Int getLength();
  
  public String join(String separator) {
    StringBuffer buffer = new StringBuffer();
    Bool first = true;
    for (T element : this) {
      if (!first)
        buffer.append(separator);
      first = false;
      buffer.append(element);
    }
    return buffer.toString();
  }
  
  public T[] getResizedCopy(Int length) {
    T[] copy = new T[length];
    if (length < getLength())
      subarray(0, length).copyTo(copy);
    else
      copyTo(copy);
    return copy;
  }
  
  public void reverse() {
    for (Int i = 0; i < (getLength() - 1) / 2; i++) {
      T temp = this[i];
      this[i] = this[getLength() - i - 1];
      this[getLength() - i - 1] = temp;
    }
  }
  
  public T[] getReverse() {
    T[] copy = getCopy();
    copy.reverse();
    return copy;
  }
}
