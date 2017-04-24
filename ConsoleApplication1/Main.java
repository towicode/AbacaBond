import java.io.*;
import java.net.*;

public class Main
{
    final static String HOST = "localhost";
    final static int    PORT = 19876;

    public static void main(String[] args) throws IOException
    {
        Socket socket = new Socket(HOST, PORT);

        // Send string to server
        PrintWriter pos = new PrintWriter(socket.getOutputStream(), true);
        pos.println("Client string to send to server");

        // Receive string from server
        BufferedReader bis =
                new BufferedReader(new InputStreamReader(socket.getInputStream()));
        System.out.println(bis.readLine());


        socket.close();
        pos.close();
        bis.close();

    }
    
}