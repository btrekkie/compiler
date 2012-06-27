final class StringBuffer {
  private Char[] chars;
  private int size;
  
  public StringBuffer(String str = "") {
    chars = new Char[2 * str.getLength() + 16];
    str.getChars().copyTo(chars);
    size = str.getLength();
  }
  
  public StringBuffer append(String str) {
    Int requiredSize = size + str.getLength();
    if (chars.getLength() < requiredSize)
      chars = chars.getResizedCopy(2 * requiredSize);
    str.getChars().copyTo(chars.subarray(size));
    size += str.getLength();
    return this;
  }
  
  public StringBuffer append(Char char) {
    if (chars.getLength() == size)
      chars = chars.getResizedCopy(2 * size + 2);
    chars[size] = char;
    size++;
  }
  
  public StringBuffer append(Object obj) {
    return append(obj.toString());
  }
  
  public String toString() {
    return new String(chars);
  }
}
