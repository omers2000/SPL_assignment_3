package bgu.spl.net.srv;

import java.util.Map;

public class Frame {
    private String command;
    private Map<String, String> headers;
    private String body;

    public Frame(String command, Map<String, String> headers, String body) {
        this.command = command;
        this.headers = headers;
        this.body = body;
    }

    public Frame(){
        this.command = null;
        this.headers = null;
        this.body = null;
    }

    public String getCommand() {
        return command;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getBody() {
        return body;
    }

    public String getKeyByHeader(String header) {
        return headers.get(header);
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public void setHeaders(Map<String, String> headers) {
        this.headers = headers;
    }

    public void setBody(String body) {
        this.body = body;
    }
}
