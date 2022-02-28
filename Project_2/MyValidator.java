public class MyValidator {

    // aceasta metoda returneaza adevarat daca intalneste un caracter de tip separator
    // aceasta metoda returneaza fals daca intalneste un caracter de tip alphanumeric
    public static boolean isSeparator(char x) {

        String separators = ";:/?~\\.,><`[]{}()!@#$%^&-_+'=*\"| \t\r\n";

        for (int i = 0 ; i < separators.length () ; i++)
        {
            if (separators.charAt(i) == x)
            {
                return true;
            }
        }
        return false;
    }
}
