public class MapTask {

    public String documentName;
    public Integer offset;
    public Integer size;
    public String sequence;
    public Integer id;

    public MapTask(String documentName, Integer offset, Integer size, String sequence, Integer id) {
        this.documentName = documentName;
        this.offset = offset;
        this.size = size;
        this.sequence = sequence;
        this.id = id;
    }
}
