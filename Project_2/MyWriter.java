import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class MyWriter {

    // se scriu datele obtinute in fisierul de output out_file
    public static void WriteResults(String out_file, ArrayList<ReduceResult> finalResults) {

        try {
            FileWriter myWriter = new FileWriter(out_file);

            // parcurgere lista cu rezultate finale
            for (int i = 0; i < finalResults.size(); i++) {

                // extragere nume document din calea catre el
                String separator = "/";
                String[] tokens = finalResults.get(i).documentName.split(separator);
                String name = "";

                // parcurgerea tuturor cuvintelor obtinute in urma 'spargerii' sirului
                for(String token : tokens) {
                    name = token;
                }
                // ultimul cuvant extras din cale reprezinta numele documentului

                myWriter.write(name + "," + String.format("%.2f", finalResults.get(i).rang) + "," + finalResults.get(i).longestWordSize + "," + finalResults.get(i).longestWordsCount + "\n");
            }

            myWriter.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
