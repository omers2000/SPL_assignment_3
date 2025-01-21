package bgu.spl.net.impl.stomp;
import java.util.HashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.User;
import bgu.spl.net.srv.Connections;


public class StompServerProtocol implements StompMessagingProtocol<Frame> {
    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<Frame> connections;

    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(Frame message) {
        switch (message.getCommand()) {
            case "DISCONNECT":
                handleDisconnect();
                break;
            case "CONNECT":
                handleConnect(message);
                break;
            case "SEND":
                handleSend(message);
                break;
            case "SUBSCRIBE":
                handleSubscribe(message);
                break;
            case "UNSUBSCRIBE":
                handleUnsubscribe(message);
                break;
            default:
                handleError(message, "Invalid command");
                break;
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void handleDisconnect() {
        // Implementation for handling disconnect
    }

    private void handleConnect(Frame message) {
        if (validateConnectFrame(message)) {
            User user = new User(message.getKeyByHeader("login"), message.getKeyByHeader("passcode"));
            if (connections.getUser(user.getUsername()) == null) {
                connections.addUser(user);
            }
            if (!connections.getUser(user.getUsername()).isLoggedIn()){
                sendConnected(message, user);
            }
            else
            handleError(message, "User already connected");
        }
    }

    private void sendConnected(Frame message, User user) {
        user.login();
        HashMap<String, String> headers = new HashMap<>();
        headers.put("version", message.getKeyByHeader("accept-version"));
        connections.send(connectionId, new Frame("CONNECTED", headers, null));
    }

    private void handleSend(Frame message) {
        // Implementation for handling send
    }

    private void handleSubscribe(Frame message) {
        // Implementation for handling subscribe
    }

    private void handleUnsubscribe(Frame message) {
        // Implementation for handling unsubscribe
    }

    private void handleError(Frame message, String errorMessage) {
        // Implementation for handling error
    }


    private boolean validateConnectFrame(Frame message) {
        if (!message.getKeyByHeader("accept-version").equals("1.2")){
            handleError(message, "Unsupported version");
        }
        else if (message.getHeaders().size() != 4){
            handleError(message, "Missing header");
        }
        else if (message.getKeyByHeader("login") == null){
            handleError(message, "Missing login");
        }
        else if (message.getKeyByHeader("passcode") == null){
            handleError(message, "Missing passcode");
        }
        else if (message.getKeyByHeader("host") == null){
            handleError(message, "Missing host");
        }
        else{
            return true;
        }
        return false;
    }

}
