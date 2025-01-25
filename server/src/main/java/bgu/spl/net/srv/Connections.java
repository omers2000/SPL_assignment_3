package bgu.spl.net.srv;

import java.io.IOException;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId, String username);

    void addConnection(int connectionId, ConnectionHandler<T> handler);

    void addUser(String username, User<T> user);

    User<T> getUser(String username);

    void addChannel(String channel);

    ConcurrentHashMap<String, HashMap<Integer, Integer>> getChannels();
    
    void cleanClient(int connectionId, String username);
}
