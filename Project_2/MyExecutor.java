import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class MyExecutor {

    // Se lanseaza un executor cu un thread pool fix, in care sunt adaugate task-urile de tip Map
    // Fiecare worker isi alege cate un task din acest pool si il va executa
    // Se asteapta o perioada de timp pentru ca toate thread-urile sa isi poata finaliza executia
    public static void ExecuteMapTasks(ArrayList<MapTask> mapTasks, int workersNumber, MapResult[] mapResults) {

        ExecutorService tpe = Executors.newFixedThreadPool(workersNumber);

        for(MapTask task : mapTasks) {
            tpe.submit(new MapWorkspace(task, mapResults));
        }

        tpe.shutdown();
        try {
            if (!tpe.awaitTermination(Long.MAX_VALUE, TimeUnit.NANOSECONDS)) {
                tpe.shutdownNow();
            }
        } catch (InterruptedException ex) {
            tpe.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }

    // Se porneste un executor cu un thread pool fix, in care sunt adaugate task-urile de tip Reduce
    // Fiecare worker isi alege cate un task din acest pool si il va executa
    // Se asteapta o perioada de timp pentru ca toate thread-urile sa isi poata finaliza executia
    public static void ExecuteReduceTasks(ArrayList<ReduceTask> reduceTasks, int workersNumber, ReduceResult[] reduceResults) {

        ExecutorService tpe = Executors.newFixedThreadPool(workersNumber);

        for(ReduceTask task : reduceTasks) {
            tpe.submit(new ReduceWorkspace(task, reduceResults));
        }

        tpe.shutdown();
        try {
            if (!tpe.awaitTermination(Long.MAX_VALUE, TimeUnit.NANOSECONDS)) {
                tpe.shutdownNow();
            }
        } catch (InterruptedException ex) {
            tpe.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }
}
