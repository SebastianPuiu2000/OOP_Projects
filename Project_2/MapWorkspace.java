import java.io.FileReader;
import java.io.IOException;

public class MapWorkspace implements Runnable {

    MapTask task;
    MapResult[] mapResults;

    public MapWorkspace(MapTask task, MapResult[] mapResults) {

        this.task = task;
        this.mapResults = mapResults;
    }

    @Override
    public void run() {

        char[] buf = new char[1];
        StringBuilder sequence = new StringBuilder();

        int semaphore = 1;
        int charactersNumber = 0;

        try {
            FileReader fr = new FileReader(task.documentName);

            if(task.offset == 0) { // cazul in care se citeste de la inceputul fisierului

                semaphore = fr.read(buf, 0, 1);

                // se citeste cat timp nu s-a atins dimensiunea speficicata si
                // daca mai exista caractere necitite in fisier
                while (semaphore == 1 && (charactersNumber < task.size - 1)) {

                    sequence.append(buf);
                    semaphore = fr.read(buf, 0, 1);
                    charactersNumber++;
                }

                // daca citirea s-a terminat in mjilocul unui cuvant :
                // se citeste cat timp nu s-a gasit un separator si
                // daca mai exista caractere necitite in fisier
                while (semaphore == 1 && (!MyValidator.isSeparator(buf[0]))) {

                    sequence.append(buf[0]);
                    semaphore = fr.read(buf, 0, 1);
                    charactersNumber++;
                }

            }
            else // cazul in care nu se citeste de la inceputul fisierului
            {
                // citirea ultimului caracter ce se afla inainte de offset
                fr.skip(task.offset - 1);
                semaphore = fr.read(buf, 0, 1);

                if(semaphore == 1) {
                    if (!MyValidator.isSeparator(buf[0])) {
                        // cazul in care caracterul citit anterior este alphanumeric
                        // deci prin urmare cel curent se afla in mijlocul unui cuvant

                        // se citeste cat timp nu s-a gasit un separator si
                        // daca mai exista caractere necitite in fisier
                        while (semaphore == 1 && (!MyValidator.isSeparator(buf[0]))) {

                            // aceste caractere vor fi ignorate, insa
                            // se tine cont cate au fost citite
                            semaphore = fr.read(buf, 0, 1);
                            charactersNumber++;
                        }

                        semaphore = fr.read(buf, 0, 1);

                        // se citeste cat timp nu s-a atins dimensiunea speficicata si
                        // daca mai exista caractere necitite in fisier
                        while (semaphore == 1 && (charactersNumber < task.size - 1)) {

                            sequence.append(buf[0]);
                            semaphore = fr.read(buf, 0, 1);
                            charactersNumber++;
                        }

                        // daca citirea s-a terminat in mjilocul unui cuvant :
                        // se citeste cat timp nu s-a gasit un separator si
                        // daca mai exista caractere necitite in fisier
                        while (semaphore == 1 && (!MyValidator.isSeparator(buf[0]))) {

                            sequence.append(buf[0]);
                            semaphore = fr.read(buf, 0, 1);
                            charactersNumber++;
                        }


                    } else {
                        // cazul in care ultimul caracter citit nu este alphanumeric
                        // deci prin urmare nu se afla in mijlocul unui cuvant

                        semaphore = fr.read(buf, 0, 1);

                        // se citeste cat timp nu s-a atins dimensiunea speficicata si
                        // daca mai exista caractere necitite in fisier
                        while (semaphore == 1 && (charactersNumber < task.size - 1)) {

                            sequence.append(buf);
                            semaphore = fr.read(buf, 0, 1);
                            charactersNumber++;
                        }

                        // daca citirea s-a terminat in mjilocul unui cuvant :
                        // se citeste cat timp nu s-a gasit un separator si
                        // daca mai exista caractere necitite in fisier
                        while (semaphore == 1 && (!MyValidator.isSeparator(buf[0]))) {

                            sequence.append(buf);
                            semaphore = fr.read(buf, 0, 1);
                            charactersNumber++;
                        }
                    }
                }
            }

            // se adauga rezultatul obtinut in lista de rezultate a operatiei Map
            mapResults[task.id] = Spliter.ComputeMapResult(task.documentName, sequence.toString());

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
