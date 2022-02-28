import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class MyReader {

    // se citesc datele de intrare din fisierul de input 'in_file'
    public static void ReadInputData(String in_file, Integer[] fragmentSize, Integer[] numberOfDocuments, ArrayList<String> documents)
            throws IOException {

        File f = new File(in_file);
        Scanner myReader = new Scanner(f);

        fragmentSize[0] = Integer.parseInt(myReader.nextLine());
        numberOfDocuments[0] = Integer.parseInt(myReader.nextLine());

        // construirea listei cu documente
        while (myReader.hasNextLine()) {
            String newDocument = myReader.nextLine();
            documents.add(newDocument);
        }

        myReader.close();
    }
}
