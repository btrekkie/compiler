/**
 * Tests the "auto" keyword.
 */
class AutoTest {
    Double getFibonacci(Double value1, Double value2, Int numIterations) {
        for (auto i = 0; i < numIterations; i++) {
            auto sum = value1 + value2;
            value1 = value2;
            value2 = sum;
        }
        return value2;
    }
    
    void testFibonacci() {
        println(getFibonacci(0.9, 1.3, 5));
        println(getFibonacci(0.4, -.9, 6));
    }
    
    Int findPrime(Int start) {
        auto isPrime = false;
        for (auto value = start; !isPrime; value++) {
            isPrime = true;
            for (auto divisor = 2; divisor <= value / 2; divisor++) {
                if (value % divisor == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime)
                return value;
        }
        return -1;
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
