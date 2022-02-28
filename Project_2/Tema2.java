import java.io.*;
import java.util.*;
import java.util.concurrent.ExecutionException;

public class Tema2 {

    public static void main(String[] args)
            throws IOException, ExecutionException, InterruptedException {

        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }

        int workersNumber = Integer.parseInt(args[0]);
        String in_file = args[1];
        String out_file = args[2];

        Integer[] documentsNumber = new Integer[1];
        Integer[] fragmentSize = new Integer[1];

        // Citire datelor din fisierul de input ce se afla pe calea primita ca argument
        ArrayList<String> documents = new ArrayList<>();
        MyReader.ReadInputData(in_file, fragmentSize, documentsNumber, documents);

        // Construirea task-urilor de tip Map
        ArrayList<MapTask> mapTasks = new ArrayList<>();
        MyCrafter.ComputeMapTasks(documents, fragmentSize[0], mapTasks);

        // Prelucrarea task-urilor de tip Map folosind ExecutorService
        MapResult[] mapResults = new MapResult[mapTasks.size()];
        MyExecutor.ExecuteMapTasks(mapTasks, workersNumber, mapResults);

        // Construirea task-urilor de tip Reduce
        ArrayList<ReduceTask> reduceTasks = new ArrayList<>();
        MyCrafter.ComputeReduceTasks(mapResults, mapTasks.size(), reduceTasks);

        // Prelucrarea task-urilor de tip Reduce folosind ExecutorService
        ReduceResult[] reduceResults = new ReduceResult[documentsNumber[0]];
        MyExecutor.ExecuteReduceTasks(reduceTasks, workersNumber, reduceResults);

        // Sortarea rezultatelor finale obtinute
        ArrayList<ReduceResult> finalResults = new ArrayList<>();
        MySorter.Sort(finalResults, documentsNumber[0], reduceResults);

        // Scrierea rezultatelor in fisierul de output ce se afla pe calea primita ca argument
        MyWriter.WriteResults(out_file, finalResults);
    }
}
