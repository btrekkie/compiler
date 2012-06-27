final class Int {
  public static Int MAX_VALUE = 2147483647;
  public static Int MIN_VALUE = -2147483648;
  
  public native Char toChar();
  public native Long toLong();
  public native Float toFloat();
  public native Double toDouble();
  
  public Int toInt() {
    return this;
  }
  
  public Int abs() {
    return this >= 0 ? this : -this;
  }
  
  public Int hash() {
    return this;
  }
  
  public String toString() {
    if (this == 0)
      return "0";
    // MIN_VALUE.abs() == MIN_VALUE, so we need to special case it
    if (this == -2147483648)
      return "-2147483648";
    
    Char[] chars = new Char[11];
    Int index = 11;
    for (Int value = abs(); value > 0; value /= 10) {
      index--;
      chars[index] = ((value % 10) + '0').toChar();
    }
    if (this < 0) {
      index--;
      buffer[index] = '-';
    }
    return new String(chars.subarray(index));
  }
}
