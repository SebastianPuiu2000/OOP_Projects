public class MyCalculator {

    // este calculata valoarea Fibonacci a numarului 'number'
    public static Integer GetFibonacciValue(Integer number) {

        int value0 = 0;
        int value1 = 1;
        int sum = 0;

        for(int i = 0; i < number; i++) {

            sum = value0 + value1;
            value0 = value1;
            value1 = sum;
        }

        return value0;

    }
}
