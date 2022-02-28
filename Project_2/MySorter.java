import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class MySorter {

    // datele prelucrate sunt copiate intr-un arraylist, pe care se aplica
    // sortarea descrescatoare a elementelor, in functie de rang-ul fiecaruia
    public static void Sort(ArrayList<ReduceResult> finalResults, int numberOfDocuments, ReduceResult[] reduceResults) {

        for(int j = 0; j < numberOfDocuments; j++) {
            finalResults.add(reduceResults[j]);
        }

        Collections.sort(finalResults, new Comparator<ReduceResult>() {

            public int compare(ReduceResult o1, ReduceResult o2) {

                if(o1.rang == o2.rang) {
                    return 0;
                }

                if(o1.rang < o2.rang) {
                    return 1;
                }

                return -1;
            }
        });
    }
}
