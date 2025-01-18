package bgu.spl.net.impl.stomp;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionHandler;

public class ConnectionsImpl<T> implements Connections<T> {
    
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> clients;
    private AtomicInteger connectionIdCounter;

    public ConnectionsImpl() {
        clients = new ConcurrentHashMap<>();
        connectionIdCounter = new AtomicInteger(0);
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        // Implementation for sending a message to a channel
    }

    @Override
    public void disconnect(int connectionId) {
        clients.remove(connectionId);
    }

    public int addConnection(ConnectionHandler<T> handler) {
        int connectionId = connectionIdCounter.incrementAndGet();
        clients.put(connectionId, handler);
        return connectionId;
    }
}
