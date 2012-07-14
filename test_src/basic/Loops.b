/**
 * Tests for loops, while loops, do-while loops, and break and continue
 * statements.
 */
class Loops {
    void testBreak() {
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++)
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                if (i + j == 3)
                    continue;
                for (var k = 1; k <= 3; k++)
                    sum += i + j + k;
            }
            sum++;
        }
        println(sum);
    }
    
    void testContinue1() {
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var sum = 0;
        for (var i = 1; i <= 3; i++) {
            for (var j = 1; j <= 3; j++) {
                for (var k = 1; k <= 3; k++) {
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
        var value = 10;
        var factorsSum;
        do {
            value++;
            factorsSum = 0;
            for (var i = 1; i < value; i++) {
                if (value % i == 0)
                    factorsSum += i;
            }
        } while (factorsSum != value);
        println(value);
    }
    
    void testTriangular() {
        var sum = 0;
        for (var i = 1; i <= 5; i++) {
            for (var j = i; j <= 5; j++) {
                for (var k = j; k <= 5; k++)
                    sum += i + j + k;
            }
        }
        println(sum);
    }
    
    void testWhile() {
        var value = 10;
        var factorsSum = 0;
        while (factorsSum != value) {
            value++;
            factorsSum = 0;
            for (var i = 1; i < value; i++) {
                if (value % i == 0)
                    factorsSum += i;
            }
        }
        println(value);
    }
}
