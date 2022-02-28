import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;

public class MyCrafter {

    // construirea task-urilor de tip Map
    public static void ComputeMapTasks(ArrayList<String> documents, int fragmentSize, ArrayList<MapTask> mapTasks) throws IOException {

        int taskId = 0;

        // sunt parcurse toate documentele
        for(int k = 0; k < documents.size(); k++) {

            FileReader fr = new FileReader(documents.get(k));
            File file = new File(documents.get(k));

            int offset = 0;
            int charactersNumber = 0;
            char[] buf = new char[1];

            StringBuilder sequence = new StringBuilder();

            // instructiunile se executa cat timp mai sunt caractere de citit in fisier
            while (charactersNumber < file.length()) {

                fr.read(buf, 0, 1);
                charactersNumber++;

                // concatenarea caracterului curent la secventa
                sequence.append(buf);

                // daca numarul de caractere citite e divizibil cu dimensiunea dorita a fragmentului
                // inseamna ca se poate construi un nou task de tip map
                if (charactersNumber % fragmentSize == 0) { // daca e multimplu de fragmente

                    MapTask task = new MapTask(documents.get(k), offset, fragmentSize,
                                    sequence.toString(), taskId);
                    taskId++;
                    mapTasks.add(task);

                    // se actualizeaza offset-ul pana la locatia din document de unde se citeste
                    offset += fragmentSize;

                } else if (charactersNumber == file.length()) {
                    // daca nu mai sunt caractere de citit in fisier
                    // inseamna ca se poate construi ultimul task de tip map cu dimensiunea obtinuta
                    // (in acest caz nu este necesar sa se atinga dimensiunea dorita a fragmentului)

                    MapTask task = new MapTask(documents.get(k), offset, charactersNumber % fragmentSize, sequence.toString(), taskId);

                    taskId++;
                    mapTasks.add(task);


                }

                // se reseteaza secventa
                sequence = new StringBuilder();
            }
        }
    }

    // construirea task-urilor de tip Reduce
    public static void ComputeReduceTasks(MapResult[] mapResults, Integer mapTasksNumber, ArrayList<ReduceTask> reduceTasks) {

        String currentDocumentName = mapResults[0].documentName;
        ArrayList<Map<Integer, Integer>> maps = new ArrayList<>();
        ArrayList<String> longestWords = new ArrayList<>();

        int resultNumber = 0;
        int taskId = 0;

        // cat timp nu s-au prelucrat toate rezultale din etapa Map
        while(resultNumber < mapTasksNumber) {

            if(currentDocumentName.equals(mapResults[resultNumber].documentName)) {
                // rezultatele din map vor fi grupate in functie de numele documentului
                // din care acestea fac parte

                // se adauga dictionarul in lista de dictionare
                maps.add(mapResults[resultNumber].map);

                // se adauga cuvintele in lista celor mai lungi cuvinte
                for(String word : mapResults[resultNumber].longestWords) {
                    longestWords.add(word);
                }

                // se incrementeaza numarul rezultatelor prelucrate
                resultNumber++;

                // daca s-a ajuns la ultimul rezultat de prelucrat
                if(resultNumber == mapTasksNumber) {
                    reduceTasks.add(new ReduceTask(currentDocumentName, maps, longestWords, taskId));
                }
            }
            else {
                // daca rezultatul curent apartine de un document diferit fata de cel anterior
                // se poate construi un nou task de tip reduce

                reduceTasks.add(new ReduceTask(currentDocumentName, maps, longestWords, taskId));
                taskId++;

                // lista de dictionare este resetata
                maps = new ArrayList<>();

                // lista de cuvinte este resetata
                longestWords = new ArrayList<>();

                // numele documentului curent se schimba
                currentDocumentName = mapResults[resultNumber].documentName;

            }
        }
    }
}
