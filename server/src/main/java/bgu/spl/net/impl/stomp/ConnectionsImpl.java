package bgu.spl.net.impl.stomp;
import java.io.IOException;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Frame;

public class ConnectionsImpl<T> implements Connections<Frame> {

    private ConcurrentHashMap<Integer, ConnectionHandler<Frame>> clients;
    private ConcurrentHashMap<String, HashMap<Integer, Integer>> channels;
    private ConcurrentHashMap<String, User<Frame>> users;
    private AtomicInteger messageCounter;

    public ConnectionsImpl() {
        clients = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        users = new ConcurrentHashMap<>();
        messageCounter = new AtomicInteger(0);
    }

    @Override
    public boolean send(int connectionId, Frame msg) {
        System.out.println("Command: " + msg.getCommand());
        for (String key : msg.getHeaders().keySet()) {
            System.out.println(key + ": " + msg.getKeyByHeader(key));
        }
        System.out.println("body:" + msg.getBody());
        System.out.println("");

        ConnectionHandler<Frame> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            System.out.println("message sent");
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, Frame msg, int connectionId) {
        if (msg.getKeyByHeader("destination").charAt(0) != '/') {
            msg.addHeader("destination", '/' + msg.getKeyByHeader("destination"));
        }
        if (channels.containsKey(channel)) {
            HashMap<Integer, Integer> subscribers = channels.get(channel);
            for (int receiverId : subscribers.keySet()) {
                if (receiverId != connectionId){
                    msg.addHeader("message-id", String.valueOf(this.messageCounter.getAndIncrement()));
                    send(receiverId, msg);
                }
            }
        }
    }

    @Override
    public void disconnect(int connectionId, String username) {
        System.out.println(connectionId + ',' + username);
        System.out.println(users.get(username) != null);
        System.out.println(users.get(username).isLoggedIn());
        users.get(username).logout();
        for (HashMap<Integer, Integer> subs : channels.values()){
            subs.remove(connectionId);
        }
    }

    @Override
    public void addConnection(int connectionId, ConnectionHandler<Frame> handler) {
        clients.put(connectionId, handler);
    }

    public void addChannel(String channel) {
        channels.putIfAbsent(channel, new HashMap<>());
    }

    public ConcurrentHashMap<String, HashMap<Integer, Integer>> getChannels(){
        return channels;
    }

    @Override
    public void addUser(String username, User<Frame> user) {
        users.put(username, user);
    }

    @Override
    public User<Frame> getUser(String username) {
        return users.get(username);
    }

    @Override
    public void cleanClient(int connectionId, String username) {
        users.get(username).logout();
        for (HashMap<Integer, Integer> subs : channels.values()){
            subs.remove(connectionId);
        }
        ConnectionHandler<Frame> handler = clients.remove(connectionId);
        handler.addTask(()->{
            try{
                handler.close();
            }
            catch(IOException x){
                x.printStackTrace();
            }
        });
    }
}
