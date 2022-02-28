import java.util.ArrayList;
import java.util.HashMap;

public class Spliter {

    // se construieste rezultatul pentru un task de tipul Map
    // este necesara aplicarea operatiei de 'split' pe secventa
    // pentru a putea obtine cuvintele ce compun rezultatul
    public static MapResult ComputeMapResult(String documentName, String sequence) {

        HashMap<Integer, Integer> map = new HashMap<Integer, Integer>();
        ArrayList<String> longestWords = new ArrayList<>();
        String separators = "[ ;:\\/?˜\\.,><‘\\[\\]{}()!@#$%ˆ&\\-+'=*”|\t\r\n]+";

        int longestWordSize = 0;

        String[] tokens = sequence.split(separators);

        // parcurgerea tuturor cuvintelor obtinute in urma 'spargerii' secventei
        for(String token : tokens) {

            // instructiunile se executa doar daca in cuvantul curent exista cel putin un caracter
            if(token.length() > 0) {

                if (map.get(token.length()) == null) { // daca nu exista aceasta cheie in map, atunci va fi adaugata
                    map.put(token.length(), 1);
                }
                else { // daca exista aceasta cheie in map, atunci valoarea ei va fi incrementata
                    map.put(token.length(), map.get(token.length()) + 1);
                }

                // daca lungimea cuvantului curent este mai mare decat a celui mai mare cuvant gasit pana acum :
                // lungimea celui mai lung cuvant va fi reactualizata
                // lista cu cele mai lungi cuvinte va fi reinitializata
                if (token.length() > longestWordSize) {

                    longestWordSize = token.length();
                    longestWords = new ArrayList<>();
                }

                // daca lungimea cuvantului curent este egala cu cea a celui mai mare cuvant gasit pana acum :
                // cuvantul curent va fi adaugat in lista celor mai lungi cuvinte
                if((token.length() == longestWordSize) && longestWordSize > 0) {

                    longestWords.add(token);
                }
            }
        }

        // se returneaza rezultatul operatiei de Map
        return new MapResult(documentName, map, longestWords);
    }
}
