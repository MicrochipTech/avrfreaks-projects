

import java.io.IOException;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.lcdui.*;
import javax.microedition.location.*;
import javax.microedition.midlet.*;


public class Midlet extends MIDlet implements CommandListener {

    private Command exitCommand,Command,Command1; // The exit command
    private Display display;     // The display for this MIDlet
    OutputStream out;
         Form t;
     StringItem item;
      String string;


    public Midlet() {
        display = Display.getDisplay(this);
        exitCommand = new Command("Exit", Command.EXIT, 0);
         Command = new Command("lampadina", Command.OK,0);
         Command1 = new Command("disco", Command.OK,0);
    }

    public void startApp() {

            t = new Form("Hello");

            t.addCommand(exitCommand);
            t.addCommand(Command);
            t.addCommand(Command1);
            t.setCommandListener(this);
            display.setCurrent(t);


            // Request the location, setting a one-minute timeout
            // Use coordinate information
            //btspp://001baff3254f:4
//BLETOOTH 00195DEE3567:1
            String url = "btspp://001210262155:1;authenticate=false;encrypt=false"; // "btspp://00158314D485:3";//computer
            try {
                StreamConnection connection = (StreamConnection) Connector.open(url, Connector.READ_WRITE);
//InputStream in = connection.openInputStream();
                out = connection.openOutputStream();
                t.append("tuto ok!");
            } catch (Exception err) {
                t.append("ERRORE DI CONNESSIONE: " + err.getMessage());
            }
}



    public void pauseApp() {
    }

    public void destroyApp(boolean unconditional) {
    }
      public void checkLocation() throws Exception
    {

        // Set criteria for selecting a location provider:




    }

    public void commandAction(Command c, Displayable s) {
        if (c == exitCommand) {
            destroyApp(false);
            notifyDestroyed();
        }
        else  if (c == Command) {
            try {
                
                out.write(1);

                out.flush();

            } catch (IOException ex) {
                ex.printStackTrace();
            }

        }
        else  if (c == Command1) {
            try {
                
                out.write(2);

                out.flush();

            } catch (IOException ex) {
                ex.printStackTrace();
            }

        }
        
    }



}
