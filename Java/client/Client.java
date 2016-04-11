
// : astyle --style=google --indent=spaces --pad-header  --lineend=linux Client.java
// : java -cp .:Java/IDL/:Java/client/ Client -ior my.ior -in hello -inout world
//
import java.io.*;

public class Client {
    private static class Args {
        final public String in;
        final public String inout;
        final public String ior;

        Args(String in, String inout, String ior) {
            this.in = in;
            this.inout = inout;
            this.ior = ior;
        }
    }

    public static Args GetArgs(String[] args) throws FileNotFoundException, IOException {
        String in, inout, ior;
        in = inout = ior = "";

        int i=0;
        while (i < args.length) {
            if (args[i].equals("-in")) {
                in = args[++i];

            } else if (args[i].equals("-inout")) {
                inout = args[++i];

            } else if (args[i].equals("-ior")) {
                final String ior_file = args[++i];
                try (BufferedReader br = new BufferedReader(new FileReader(ior_file)) ) {
                    ior = br.readLine();
                }
            }

            ++i;
        }

        return new Args(in, inout, ior);
    }


    public static void main(String[] args) {
        try {
            // obtain arguments
            final Args a = GetArgs(args);
            String in_val = a.in;
            org.omg.CORBA.StringHolder inout_val = new org.omg.CORBA.StringHolder(a.inout);
            org.omg.CORBA.StringHolder out_val = new org.omg.CORBA.StringHolder();

            // initialize the ORB
            org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args, null);

            // obtain an object reference
            Messenger messenger = MessengerHelper.narrow(orb.string_to_object(a.ior));

            // invoke the method
            messenger.string_op(in_val, inout_val, out_val);
            System.out.println("Messenger::string_op() input: " + in_val
                               + " - in/output: " + inout_val.value
                               + " - output: " + out_val.value);

            // cleanup
            orb.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}



