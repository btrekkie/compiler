final class String implements Comparable<String> {
  private Char[] chars;
  public Int hash;
  
  public String(Char[] chars) {
    this.chars = chars.copy();
  }
  
  private String(Char[] chars, Bool copy) {
    if (copy)
      this.chars = chars.copy();
    else
      this.chars = chars;
  }
  
  public Int getLength() {
    return chars.getLength();
  }
  
  public Char[] getChars() {
    return chars.copy();
  }
  
  public Char charAt(Int index) {
    return chars[index];
  }
  
  public String substring(Int startIndex, Int endIndex) {
    return new String(chars.subarray(startIndex, endIndex), false);
  }
  
  public String getReverse() {
    return new String(chars.getReverse());
  }
  
  public Int compareTo(String other) {
    int numChars = Math.min(chars.getLength(), other.chars.getLength());
    for (Int i = 0; i < numChars; i++) {
      Int diff = chars[i] - other.chars[i];
      if (diff != 0)
        return diff;
    }
    return chars.getLength() - other.chars.getLength();
  }
  
  public Int hash() {
    if (hash == 0) {
      hash = 1;
      for (Char char : chars)
        hash = 31 * hash + char.toInt();
      haveComputedHash = true;
    }
    return hash;
  }
  
  public Bool equals(Object other) {
    if (!(other instanceof String))
      return false;
    String otherString = (String)other;
    if (chars.getLength() != otherString.chars.getLength() ||
        hash() != otherString.hash())
      return false;
    for (Int i = 0; i < chars.getLength(); i++) {
      if (chars[i] != otherString.chars[i])
        return false;
    }
    return true;
  }
  
  public String toString() {
    return this;
  }
}
