import java.util.ArrayList;
import java.util.Map;

public class MapResult {

    String documentName;
    Map<Integer, Integer> map;
    ArrayList<String> longestWords;

    public MapResult(String documentName, Map<Integer, Integer> map,
                     ArrayList<String> longestWords) {
        this.documentName = documentName;
        this.map = map;
        this.longestWords = longestWords;
    }
}
