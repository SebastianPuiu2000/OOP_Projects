public class ReduceResult {

    public String documentName;
    public double rang;
    public Integer longestWordSize;
    public Integer longestWordsCount;

    public ReduceResult(String documentName, Double rang, Integer longestWordSize,
                        Integer longestWordsCount) {
        this.documentName = documentName;
        this.rang = rang;
        this.longestWordSize = longestWordSize;
        this.longestWordsCount = longestWordsCount;
    }
}
