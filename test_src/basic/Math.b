/**
 * Assorted tests pertaining to math.
 */
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
    
    Int addFractions(
        Int numerator1,
        Int denominator1,
        Int numerator2,
        Int denominator2) {
        Int numerator = numerator1 * denominator2 + numerator2 * denominator1;
        Int denominator = denominator1 * denominator2;
        Int gcd = gcd(numerator, denominator);
        return numerator / gcd + denominator / gcd;
    }
    
    void testFractions() {
        println(addFractions(1, 2, 1, 3));
        println(addFractions(5, 8, 7, 12));
        println(addFractions(3, 5, 7, 15));
    }
    
    Int gcd(Int x, Int y) {
        if (x < y) {
            Int temp = x;
            x = y;
            y = temp;
        }
        while (y > 0) {
            Int temp = x % y;
            x = y;
            y = temp;
        }
        return x;
    }
    
    void testGCD() {
        println(gcd(1, 9));
        println(gcd(128, 100));
        println(gcd(12, 15));
        println(gcd(23456, 12345));
    }
    
    Float sqrtf(Float value) {
        Float sqrt = 2;
        for (Int i = 0; i < 5; i++)
            sqrt = (sqrt + value / sqrt) / 2;
        return sqrt;
    }
    
    Double sqrtd(Double value) {
        Double sqrt = 2;
        for (Int i = 0; i < 5; i++)
            sqrt = (sqrt + value / sqrt) / 2;
        return sqrt;
    }
    
    void testSqrt() {
        println(sqrtf(17));
        println(sqrtd(17));
        println(sqrtf(1));
        println(sqrtd(1));
        println(sqrtf(12345));
        println(sqrtd(12345));
    }
}
