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
    private String username;

    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
        this.username = null;
    }

    @Override
    public void process(Frame message) {

        System.out.println("Command: " + message.getCommand());
        for (String key : message.getHeaders().keySet()) {
            System.out.println(key + ": " + message.getKeyByHeader(key));
        }
        System.out.println("Body: " + message.getBody());
        System.out.println("");

        switch (message.getCommand()) {
            case "DISCONNECT":
                handleDisconnect(message);
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
                sendError(message, "Invalid command", "The command '" + message.getCommand() + "' is not recognized by the server. Please check the command and try again.");
                break;
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void handleDisconnect(Frame message) {
        connections.disconnect(connectionId, username);
        sendReceipt(message.getKeyByHeader("receipt"));
    }

    private void handleConnect(Frame message) {
        if (validateConnectFrame(message)) {
            User<Frame> user = new User<Frame>(message.getKeyByHeader("login"), message.getKeyByHeader("passcode"));
            this.username = user.getUsername();
            if (connections.getUser(username) == null) {
                connections.addUser(username, user);
            }
            if (connections.getUser(username).getPassword() != user.getPassword()){
                sendError(message, "Wrong passcode", "You entered the wrong passcode for the user '" + username + "'. Please check the passcode and try again.");
            }
            else if (connections.getUser(username).isLoggedIn()){
                sendError(message, "User already connected", "The user '" + username + "' is already connected. Please disconnect the current session before connecting again.");
            }
            else {
                user.login();
                sendConnected(message);
            }
        }
    }

    private void handleSend(Frame message) {
        String channel = message.getKeyByHeader("destination");
        if (channel.charAt(0) == '/') {
            channel = channel.substring(1);
        }
        if (connections.getChannels().get(channel) == null){
            sendError(message, "Channel does not exist", "The channel '" + channel + "' does not exist. Please check the destination and try again.");
        }
        else if (connections.getChannels().get(channel).get(connectionId) == null){
            sendError(message, "Not subscribed", "You are not subscribed to the channel '" + channel + "'. Please subscribe to the channel and try again.");
        }
        else {
            sendMessage(message, channel);
            if (message.getKeyByHeader("receipt") != null){
                sendReceipt(message.getKeyByHeader("receipt"));
            }
        }
    }

    private void handleSubscribe(Frame message) {
        String channel = message.getKeyByHeader("destination");
        if (channel.charAt(0) == '/') {
            channel = channel.substring(1);
        }
        connections.addChannel(channel);
        HashMap<Integer, Integer> channelSubs = connections.getChannels().get(channel);
        if (channelSubs.containsKey(connectionId)) {
            sendError(message, "User already subscribed", "The user '" + username + "' is already subscribed to channel '" + channel + "'. Please reconnect and send different command.");
        }
        else {
            channelSubs.put(connectionId, Integer.parseInt(message.getKeyByHeader("id")));
            if (message.getKeyByHeader("receipt") != null){
                sendReceipt(message.getKeyByHeader("receipt"));
            }
            System.out.println("subscribed user to channel");
        }
    }

    private void handleUnsubscribe(Frame message) {
        boolean done = false;
        String id = message.getKeyByHeader("id");
        for (HashMap<Integer, Integer> subs : connections.getChannels().values()){
            if (subs.containsKey(connectionId) && subs.get(connectionId).toString().equals(id)){
                subs.remove(connectionId);
                done = true;
                if (message.getKeyByHeader("receipt") != null){
                    sendReceipt(message.getKeyByHeader("receipt"));
                }
                System.out.println("user unsubscribed");
                break;
            }
        }
        if (!done){
            sendError(message, "Subscription not found", "You were not subscribed to any channel with id '" + id + "'. Please check the 'id' and try again.");
        }
    }

    /**
     * Sends a CONNECTED frame to the client.
     *
     * @param message the received message
     */
     private void sendConnected(Frame message) {
        HashMap<String, String> headers = new HashMap<>();
        headers.put("version", message.getKeyByHeader("accept-version"));
        connections.send(connectionId, new Frame("CONNECTED", headers, ""));
    }

    /**
     * Sends a MESSAGE frame to the specified channel.
     *
     * @param message the SEND frame that got from the client
     * @param channel the channel to send the message to
     */    
    private void sendMessage(Frame message, String channel) {
        HashMap<String, String> headers = new HashMap<>();
        headers.put("subscription", connections.getChannels().get(channel).get(connectionId).toString());
        headers.put("destination", channel);
        connections.send(channel, new Frame("MESSAGE", headers, message.getBody()));
    }

    /**
     * Sends an ERROR frame to the client.
     *
     * @param message     the received message
     * @param errorMessage the error message
     * @param errorDesc   the error description
     */
    private void sendError(Frame message, String errorMessage, String errorDesc) {
        HashMap<String, String> headers = new HashMap<>();
        if (message.getKeyByHeader("receipt") != null){
            headers.put("receipt-id", message.getKeyByHeader("receipt"));
        }
        headers.put("message", errorMessage);
        connections.send(connectionId, new Frame("ERROR", headers, buildErrorBody(message, errorDesc)));
        connections.cleanClient(connectionId, username);
        shouldTerminate = true;
    }

    /**
     * send a receipt to the client if he specified that in the command
     * @param receiptId reciept-id that made by the client
     */
    private void sendReceipt(String receiptId){
        HashMap<String, String> headers = new HashMap<>();
        headers.put("receipt-id", receiptId);
        connections.send(connectionId, new Frame("RECEIPT", headers, ""));
    }

    /**
     * build the body for the ERROR frame
     * @param message the received message
     * @param errorDesc the error description
     * @return String that represents the body for the ERROR frame
     */
    private String buildErrorBody(Frame message, String errorDesc){
        StringBuilder bodyBuilder = new StringBuilder();
        bodyBuilder.append("The message:\n-----\n");
        bodyBuilder.append(message.getCommand()).append("\n");
        for (HashMap.Entry<String, String> entry : message.getHeaders().entrySet()) {
            bodyBuilder.append(entry.getKey()).append(": ").append(entry.getValue()).append("\n");
        }
        bodyBuilder.append("\n").append(message.getBody()).append("\n");
        bodyBuilder.append("-----\n");
        bodyBuilder.append(errorDesc);

        return bodyBuilder.toString();
    }

    /**
     * Validates the CONNECT frame.
     *
     * @param message the received message
     * @return true if the CONNECT frame is valid, false otherwise
     */
    private boolean validateConnectFrame(Frame message) {
        if (!message.getKeyByHeader("accept-version").equals("1.2")){
            sendError(message, "Unsupported version", "The server supports version 1.2 only. Please use version 1.2 and try again.");
        }
        else if (message.getHeaders().size() != 4){
            sendError(message, "Missing header", "The connect frame must contain exactly 4 headers: 'accept-version', 'login', 'passcode', and 'host'.");
        }
        else if (message.getKeyByHeader("login") == null){
            sendError(message, "Missing login", "The 'login' header is missing. Please provide the 'login' header and try again.");
        }
        else if (message.getKeyByHeader("passcode") == null){
            sendError(message, "Missing passcode", "The 'passcode' header is missing. Please provide the 'passcode' header and try again.");
        }
        else if (message.getKeyByHeader("host") == null){
            sendError(message, "Missing host", "The 'host' header is missing. Please provide the 'host' header and try again.");
        }
        else{
            return true;
        }
        return false;
    }

}
