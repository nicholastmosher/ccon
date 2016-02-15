import java.util.*;

/**
 * Created by Nick Mosher on 1/8/16.
 */
public class Ccon {

    private static List<CommandEntry> mCommandTable = new ArrayList<>();

    public static void main(String[] args) {

        if(args.length < 1) {
            System.out.println(usage());
            return;
        }

        switch(args[0]) {
            case "compress":
                if(args.length == 2) {
                    System.out.println("From: " + args[1]);
                    System.out.println("To: " + compress(args[1]));
                }
                break;
            case "lookup":

                break;
            default:
        }
    }

    public static String compress(String notation) {
        String compressed = "";

        String[] elements = notation.split("<");
        for(String s : elements) {
            if(s.split(">").length != 2) {
                System.out.println("Improper formatting: Found too many '>'s");
                return "Error";
            }
            String key = s.split(">")[0];
            List<String> args = Arrays.asList(s.split(">")[1].split(","));
            CommandEntry entry = new CommandEntry(key, args);
            mCommandTable.add(entry);
            compressed += mCommandTable.size()-1 + ">";
            for(int i = 0; i < entry.arguments.size(); i++) {
                compressed += i + ":" + entry.arguments.get(i).split(":")[1] + ",";
            }
            compressed = compressed.substring(0, compressed.length()-1);
            compressed += "<";
        }
        return compressed;
    }

    public static String usage() {
        return "Usage: ccon [compress <data>] [lookup <key>]";
    }

    private static class CommandEntry {
        public String key;
        public final List<String> arguments;
        public CommandEntry(String key, List<String> args) {
            this.key = key;
            arguments = args;
        }
    }
}
