package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Frame;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompMessageEncoderDecoder implements MessageEncoderDecoder<Frame> {

    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;
    private byte prevByte = -1;
    private Frame result = new Frame();
    private Section currentSection = Section.COMMAND;
    private String headerName = null;
    private String headerVal = null;

    private enum Section {
        COMMAND, HEADERS, BODY
    }

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == '\n' & currentSection == Section.COMMAND) {
            this.result.setCommand(popString());
            this.currentSection = Section.HEADERS;
        }
        else if (nextByte == ':' & currentSection == Section.HEADERS) {
            this.headerName = popString();
        }
        else if (nextByte == '\n' & currentSection == Section.HEADERS & headerName != null) {
            this.headerVal = popString();
            this.result.addHeader(headerName, headerVal);
            this.headerName = null;
        }
        else if (nextByte == '\n' & currentSection == Section.HEADERS & prevByte == '\n') {
            this.currentSection = Section.BODY;
        }
        else if (nextByte == '\u0000' & currentSection == Section.BODY) {
            this.result.setBody(popString());
            return result;
        }
        else {
            pushByte(nextByte);
        }

        prevByte = nextByte;

        return null;
    }

    @Override
    public byte[] encode(Frame message) {
        return frameString(message).getBytes();
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        bytes = new byte[1 << 10];
        return result;
    }

    /**
     * Convert a frame to a string
     * @param frame the frame to convert
     * @return the string representation of the frame
     */
    private String frameString(Frame frame) {
        String result = frame.getCommand() + "\n";
        for (String key : frame.getHeaders().keySet()) {
            result += key + ":" + frame.getHeaders().get(key) + "\n";
        }
        result += "\n" + frame.getBody() + "\u0000";
        return result;
    }
}