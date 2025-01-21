package bgu.spl.net.impl.stomp;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;
import bgu.spl.net.srv.ConnectionHandler;

public class ConnectionsImpl<Frame> implements Connections<Frame> {

    private ConcurrentHashMap<Integer, ConnectionHandler<Frame>> clients;
    private ConcurrentHashMap<String, List<Integer>> channels;
    private ConcurrentHashMap<String, User> users;

    public ConnectionsImpl() {
        clients = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        users = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, Frame msg) {
        ConnectionHandler<Frame> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, Frame msg) {
        if (channels.containsKey(channel)) {
            List<Integer> subscribers = channels.get(channel);
            for (int connectionId : subscribers) {
                // Todo: change message to be with the connectionId
                send(connectionId, msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        clients.remove(connectionId);
    }

    @Override
    public void addConnection(int connectionId, ConnectionHandler<Frame> handler) {
        clients.put(connectionId, handler);
    }

    @Override
    public void addChannel(String channel, int connectionId) {
        channels.putIfAbsent(channel, new CopyOnWriteArrayList<>());
        channels.get(channel).add(connectionId);
    }

    @SuppressWarnings("rawtypes")
    @Override
    public void addUser(User user) {
        users.put(user.getUsername(), user);
    }

    @SuppressWarnings("rawtypes")
    @Override
    public User getUser(String username) {
        return users.get(username);
    }
}
