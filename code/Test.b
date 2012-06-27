class Test {
  Int start = 42;
  
  Int findPrime(Int start) {
    Bool isPrime = true;
    Int value;
    for (value = start; !isPrime; value++) {
      isPrime = true;
      for (Int divisor = 2; divisor < start / 2; divisor++) {
        if (value % divisor == 0) {
          isPrime = false;
          break;
        }
      }
    }
    return value;
  }
}
