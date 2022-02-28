import java.util.ArrayList;
import java.util.Map;

public class ReduceTask {

    public String documentName;
    public ArrayList<Map<Integer, Integer>> maps;
    public ArrayList<String> longestWords;
    public Integer id;

    public ReduceTask(String documentName,
                      ArrayList<Map<Integer, Integer>> maps,
                      ArrayList<String> longestWords,
                      Integer id) {
        this.documentName = documentName;
        this.maps = maps;
        this.longestWords = longestWords;
        this.id = id;
    }
}
