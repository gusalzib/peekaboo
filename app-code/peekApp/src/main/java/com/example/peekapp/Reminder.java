package com.example.peekapp;

public class Reminder {
    // made the attributes public because otherwise the Gson cannot access them if we want to
    // convert the Reminder object to a Json object
    public String content;
    public int charLength;


    public Reminder(String content){
        this.content = content;
        this.charLength = content.length();
    }

    public void setContent(String content) {
        this.content = content;
    }

    public String getContent() {
        return content;
    }

    // we can allow a getter for the content length but not setters since it should eb the length of the content and shouldn't be set by any external users
    public int getCharLength() {
        return charLength;
    }


    public boolean checkLength(String content){ // the assumption is that any of the screens we have has limited space, thus, only a certain length should be allowed.
        int length = content.length();
        return length < 70;
    }

    @Override
    public String toString(){
        return "Reminders:" + this.content;
    }
}
