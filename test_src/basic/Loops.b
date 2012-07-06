class Loops {
    void testBreak() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++)
                    sum += i + j + k;
                if (i + j == 3)
                    break;
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testBreak1() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        break 1;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testBreak2() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        break 2;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testBreak3() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        break 3;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testContinue() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                if (i + j == 3)
                    continue;
                for (Int k = 1; k <= 3; k++)
                    sum += i + j + k;
            }
            sum++;
        }
        println(sum);
    }
    
    void testContinue1() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        continue 1;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testContinue2() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        continue 2;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testContinue3() {
        Int sum = 0;
        for (Int i = 1; i <= 3; i++) {
            for (Int j = 1; j <= 3; j++) {
                for (Int k = 1; k <= 3; k++) {
                    if (i + j + k == 5)
                        continue 3;
                    sum += i + j + k;
                }
                sum++;
            }
            sum++;
        }
        println(sum);
    }
    
    void testDoWhile() {
        Int value = 10;
        Int factorsSum;
        do {
            value++;
            factorsSum = 0;
            for (Int i = 1; i < value; i++) {
                if (value % i == 0)
                    factorsSum += i;
            }
        } while (factorsSum != value);
        println(value);
    }
    
    void testTriangular() {
        Int sum = 0;
        for (Int i = 1; i <= 5; i++) {
            for (Int j = i; j <= 5; j++) {
                for (Int k = j; k <= 5; k++)
                    sum += i + j + k;
            }
        }
        println(sum);
    }
    
    void testWhile() {
        Int value = 10;
        Int factorsSum = 0;
        while (factorsSum != value) {
            value++;
            factorsSum = 0;
            for (Int i = 1; i < value; i++) {
                if (value % i == 0)
                    factorsSum += i;
            }
        }
        println(value);
    }
}
