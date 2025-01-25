package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("missing arguments");
            return;
        }
        int port = Integer.parseInt(args[0]);
        String serverType = args[1];

        ConnectionsImpl<Frame> connections = new ConnectionsImpl<>();

        if (serverType.equals("tpc")) {
            Server.threadPerClient(
                port,
                () -> new StompServerProtocol(),
                StompMessageEncoderDecoder::new,
                connections
            ).serve();
        } else if (serverType.equals("reactor")) {
            Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                port,
                () -> new StompServerProtocol(),
                StompMessageEncoderDecoder::new,
                connections
            ).serve();
        } else {
            System.out.println("Invalid server type");
        }
    }
}
