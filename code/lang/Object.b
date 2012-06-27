class Object {
  public native Int hash();
  public native String toString();

  public Bool equals(Object other) {
    return this == other;
  }
}
