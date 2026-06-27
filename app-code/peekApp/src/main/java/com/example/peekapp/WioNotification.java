package com.example.peekapp;

import java.util.List;

public class WioNotification {

    public List<Reminder> reminders;

    public WioNotification(List<Reminder> reminders) {

        this.reminders = reminders;

    }

    public static class Reminder {
        // made the attributes public because otherwise the Gson cannot access them if we want to
        // convert the Reminder object to a Json object
        // this is the error message " Failed making field 'com.example.peekapp.WioNotification$Reminder#text' accessible;
        // either increase its visibility or write a custom TypeAdapter for its declaring type."

        public String text;

        public Reminder(String text) {
            this.text = text;
        }


        @Override
        public String toString(){
            return  this.text;
        }
    }

}
