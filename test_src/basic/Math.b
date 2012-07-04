class Math {
    Int getFibonacci(Int n) {
        return n > 1 ? (getFibonacci(n - 1) + getFibonacci(n - 2)) : 1;
    }
    
    void testFibonacci() {
         println(getFibonacci(0));
         println(getFibonacci(1));
         println(getFibonacci(3));
         println(getFibonacci(7));
    }
    
    Int findPrime(Int start) {
        Bool isPrime = false;
        Int value;
        for (value = start; !isPrime; value++) {
            isPrime = true;
            for (Int divisor = 2; divisor <= value / 2; divisor++) {
                if (value % divisor == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime)
                break;
        }
        return value;
    }
    
    void testFindPrime() {
        println(findPrime(2));
        println(findPrime(3));
        println(findPrime(4));
        println(findPrime(12));
        println(findPrime(49));
        println(findPrime(102));
    }
}
