package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Frame;

import bgu.spl.net.srv.Connections;


public class StompServerProtocol implements StompMessagingProtocol<Frame> {
    private boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        // Initialize the protocol with the connection ID and connections
        int i = connectionId;
    }

    @Override
    public void process(Frame message) {
        // Process the received message
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
