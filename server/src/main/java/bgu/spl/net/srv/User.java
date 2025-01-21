package bgu.spl.net.srv;

import java.util.concurrent.atomic.AtomicBoolean;

public class User<T> {
    private String username;
    private String password;
    private AtomicBoolean isLoggedIn;

    public User(String username, String password) {
        this.username = username;
        this.password = password;
        this.isLoggedIn = new AtomicBoolean(false);
    }

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public boolean isLoggedIn() {
        return isLoggedIn.get();
    }

    public void login() {
        isLoggedIn.set(true);
    }

    public void logout() {
        isLoggedIn.set(false);
    }

    public boolean validatePassword(String password) {
        return this.password.equals(password);
    }
}