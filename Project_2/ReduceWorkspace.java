import java.util.Map;

public class ReduceWorkspace implements Runnable {

    ReduceTask task;
    ReduceResult[] reduceResults;

    public ReduceWorkspace(ReduceTask task, ReduceResult[] reduceResults) {

        this.task = task;
        this.reduceResults = reduceResults;
    }

    @Override
    public void run() {

        int longestWordSize = 0;
        int appearances = 0;
        double sum = 0.0;
        double rang = 0.0;
        double wordCount = 0.0;

        // se parcurg map-urile obtinute in task-ul curent
        for(Map<Integer, Integer> map : task.maps) {
            // se verifica ca map-ul sa nu fie gol
            if(map.size() != 0) {
                // se parcurg toate perechile cheie-valoare din map-ul curent
                for (Map.Entry<Integer, Integer> entry : map.entrySet()) {

                    if (longestWordSize < entry.getKey()) {
                        // daca cea mai mare lungime gasita pana acum este mai mica decat cheia curenta
                        // este actualizata lungimea cea mai mare si numarul de aparitii devine
                        // valoarea asociata cheii curente
                        longestWordSize = entry.getKey();
                        appearances = entry.getValue();
                    }
                    else if (longestWordSize == entry.getKey()) {
                        // in caz contrar, daca cele doua sunt egale, la numarul de apartii al
                        // lungimii maxime va fi adaugata valoarea cheii curente
                        appearances += entry.getValue();
                    }

                    // 'sum' este calculat conform formulei din pdf
                    sum += MyCalculator.GetFibonacciValue(entry.getKey() + 1) * entry.getValue();
                    // 'wordCount' reprezinta numarul total de cuvinte
                    wordCount += entry.getValue();

                }
            }
        }

        // rang-ul este calculat conform formulei din pdf
        rang = sum / wordCount;

        // se adauga rezultatul obtinut in lista de rezultate a operatiei Reduce
        reduceResults[task.id] = new ReduceResult(task.documentName, rang, longestWordSize, appearances);
    }
}
