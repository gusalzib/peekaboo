package com.example.peekapp;

import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.Stage;

import com.google.gson.Gson;

import java.io.*;
import java.net.URL;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.ResourceBundle;


public class Controller implements Initializable {

    final String LINE_SEPARATOR = System.lineSeparator();

    @FXML
    TextField latitudeTextField;
    @FXML
    TextField longitudeTextField;
    @FXML
    Label settingsWarningLabel;
    @FXML
    Label tempLabelCel;
    @FXML
    Label tempLabelFar;
    @FXML
    Label humidityLabel;
    @FXML
    Label warningText;
    @FXML
    TextField addReminderTextfield;
    @FXML
    ListView<WioNotification.Reminder> remindersListView;
    @FXML
    Label remindersWarningLabel;
    @FXML
    Label bathroomAvailable;
    @FXML
    ImageView emoji;



    private final String TEMP_C = "bathroom/celsius";
    private final String TEMP_F = "bathroom/fahrenheit";
    private final String HUMI = "bathroom/humi";
    private final String REMINDER = "reminder";
    private final String LOCATION = "bathroom/location";
    private final String MOTION_TOPIC = "bathroom/motion";



    @FXML
    private Stage stage;
    @FXML
    private Scene scene;


    /*Reminders variables
     * https://openjfx.io/javadoc/20/javafx.controls/javafx/scene/control/ListView.html */
    ObservableList<WioNotification.Reminder> observableListReminders;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        updateTemp();
        updateHumidity();
        checkBathroomAvailability();

        try {
            readReminderFromJson(new ActionEvent());
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        /*initialize reminders-related variables and lists because otherwise they would be overwritten constantly*/
        observableListReminders = FXCollections.observableArrayList();
    }
    public void getScene(String path, ActionEvent event) throws Exception { //general method I made to make switching scenes easier.
        Parent root = FXMLLoader.load(getClass().getResource(path));
        stage = (Stage) (((Node)event.getSource()).getScene().getWindow());
        scene = new Scene(root);
        stage.setScene(scene);
        stage.show();
    }

    public void back(ActionEvent event) {
        try {
            getScene("mainVid.fxml", event);
        }catch (Exception e){
            System.out.println("Could not load main screen!");
            e.printStackTrace();
        }
    }

    /*Start--------------------------------------------------------Main screen------------------------------------------------------------------------*/
    public void switchToStatus(ActionEvent event) { //statusBtn
        try {
            getScene("statusScene.fxml", event);
        }catch (Exception e){
            System.out.println("Could not load status screen!");
            e.printStackTrace();
        }
    }
    public void switchToSettings(ActionEvent event) { //statusBtn
        try {
            getScene("settings.fxml", event);

        }catch (Exception e){
            System.out.println("Could not load settings screen!");
            e.printStackTrace();
        }

    }
    public void switchToReminders(ActionEvent event) { //remindersBtn
        try {
            getScene("reminderScene.fxml", event);
            readReminderFromJson(new ActionEvent()); // update the remindersList (ListView element) as we go to the reminders screen
        }catch (Exception e){
            System.out.println("Could not load reminders screen!");
            e.printStackTrace();

        }

    }

    /*End--------------------------------------------------------Main screen------------------------------------------------------------------------*/

    /*Start--------------------------------------------------------Settings screen------------------------------------------------------------------------*/

    public void getWeatherSettings(){
        /*1. get the user input as strings
         * 2. check if any of the strings is empty and display a warning if so
         * 3. parse string to float. (this is a preemptive check because the Wio is expecting the payload to be convertable to float format). throws an exception and displays a warning message in case parsing failed
         * 4. then passes the coordinates to a method that checks their validity and displays the appropriate message in case something is wrong
         * 5. finally, if everything is correct, the coordinates are sent to the Broker. */
        String longitudeStr = longitudeTextField.getText();
        String latitudeStr = latitudeTextField.getText();
        float longitudeFloat = 0;
        float latitudeFloat = 0;
        boolean emptyFields = longitudeStr.isBlank() || latitudeStr.isBlank();
        boolean parsingSuccessful = false;

        if (emptyFields){
            settingsWarningLabel.setText("Please fill in both text field!");
            return; // the method has multiple exit points because otherwise the warning messages will stack on top of each other and the user will not be getting the correct warning message.
        }else{

            try{
                longitudeFloat= Float.parseFloat(longitudeStr);
                latitudeFloat= Float.parseFloat(latitudeStr);
                parsingSuccessful = true;
            }catch (Exception exception){
                settingsWarningLabel.setText("Wrong format!");
                return;
            }
        }

        if (!validCoordinates(longitudeFloat, latitudeFloat)){
            settingsWarningLabel.setText("Invalid coordinates!");

        }else if (parsingSuccessful && !emptyFields){
            String payload = String.format("%09.6f%09.6f", latitudeFloat,longitudeFloat );
            MqttConnection publish = new MqttConnection(LOCATION);
            publish.publishPayload(LOCATION, payload);
            settingsWarningLabel.setText("");
        }
    }

    public boolean validCoordinates(float longitude, float latitude){
        boolean invalidLongitude = longitude < 2.25 || longitude > 27.3;
        boolean invalidLatitude = latitude > 70.6 || latitude < 52.5;

        if (invalidLongitude || invalidLatitude){
            return false;
        }else {
            return true;
        }
    }
    /*End----------------------------------------------------------Settings screen------------------------------------------------------------------------*/

    /*Start--------------------------------------------------------Reminders screen------------------------------------------------------------------------*/
    public void getUserInput() throws IOException {
        int maxReminders = 2;
        int maxChar = 23;
        String text = addReminderTextfield.getText();

        //check if reminder char length is appropriate and that the max number of reminders is not exceeded
        if (text.length() > maxChar){
            remindersWarningLabel.setText("Reminder should be less than 24 characters. This reminder is: "+text.length());
        }else {
            WioNotification.Reminder reminder = new WioNotification.Reminder(text);

            if (observableListReminders.size() > maxReminders){
                remindersWarningLabel.setText("There cannot be more than 3 reminders!");
            }else{
                remindersWarningLabel.setText("");

                if (!text.equals("")) {
                    observableListReminders.add(reminder);
                    remindersListView.setItems(observableListReminders);//this line is responsible for displaying the added reminders on the Listview element in the UI
                    writeReminderToFile("src/main/resources/json/reminders.JSON", reminder.text);
//                    writeReminderToFile("src/main/resources/com/example/peekapp/reminders.JSON", reminder.text);
                } else if (text.equals("")){
                    remindersWarningLabel.setText(" The reminder should not be empty.");
                }
            }
            addReminderTextfield.clear();
        }
    }
    /*this method takes a ready-made json string and overwrites the file.
    used when we remove a reminder. The list of reminders gets updated and then instead of looping through the
    json file and searching for the reminder to be removed, we just get an updated list of reminders and overwrite the file contents.
    Since we are not going to have many reminders, this is a practical solution, however, if the reminders list is going to
    be long, then this might not be the optimal solution. */
    public void writeJsonToFile(String fileName, String json) throws IOException {

        FileWriter reminderFile = new FileWriter(fileName, false);

        reminderFile.write(json);
        reminderFile.write(LINE_SEPARATOR);
        reminderFile.close();
    }

    // this method takes a single reminder and adds to the json array of reminders
    public void writeReminderToFile(String fileName, String content) throws IOException {
        /*when writing to the json file, we don't want to program to duplicate the listView items, that's why
         * we need to convert the reminders that are in the json file back into an ArrayList, remove or add the
         * desired item and then overwrite the json file by writing to it the new and modified ArrayList of reminders*/
        WioNotification.Reminder tempReminder = new WioNotification.Reminder(content);
        FileReader fileReader = new FileReader(fileName);
        Gson gson = new Gson();
        ArrayList<WioNotification.Reminder> notificationsList = new ArrayList<>();

        WioNotification.Reminder[] notifications = gson.fromJson(fileReader, WioNotification.Reminder[].class);
        notificationsList.addAll(Arrays.asList(notifications));
        notificationsList.add(tempReminder);
        notifications = notificationsList.toArray(new WioNotification.Reminder[0]);
        observableListReminders = FXCollections.observableArrayList(notifications); //overwrite the old observable list with a new one where the selected element is removed

        // Json & FileWriter & FileReader tutorial
        //https://www.baeldung.com/
        FileWriter reminderFile = new FileWriter(fileName, false); //the false means that we want to overwrite the file
        String json = gson.toJson(notifications);
        /*we need the line separator afterward so that we get each object on a new line.
         * Check the Json folder in the resources folder in the repo. There is a file called reminders.json.
         * Everything typed in the text field in reminders scene is stored there. */
        reminderFile.write(json);
        reminderFile.write(LINE_SEPARATOR);
        reminderFile.close();
        /* After writing the reminder content/text, the user has to press the "Add"  button
         * This means that a new FileWriter object is initialized and whatever is typed into the text field
         * is stored in the reminder.text attribute. Then, after storing the content, the FileWriter needs to
         * be closed so that the changes are flushed (i.e. saved) and the resources are let go. This is what I
         * understood from this guide:
         * https://www.baeldung.com/java-filewriter-flush-vs-close#:~:text=The%20Difference%20Between%20the%20flush,flushing%20and%20releases%20associated%20resources.*/
        MqttConnection publish = new MqttConnection(REMINDER);
        publish.publishPayload(REMINDER, json);
    }

    public void readReminderFromJson(ActionEvent event) {
        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                try {
                    FileReader fileReader = new FileReader("src/main/resources/json/reminders.JSON");
//
//                    File file = new File("src/main/resources/com/example/peekapp/reminders.JSON");
//                    FileReader fileReader = new FileReader(file);
                    Gson gson = new Gson();

                    WioNotification.Reminder[] notificationsArray = gson.fromJson(fileReader, WioNotification.Reminder[].class);

                    observableListReminders.addAll(notificationsArray);
                    if (remindersListView != null){
                        remindersListView.setItems(observableListReminders);//the problem is in this line. For some reason it is throwing a NullPointerException
                    }

                }catch (Exception e){
                    System.out.println("Could not read from file. File not found!");
                    e.printStackTrace();
                }
            }
        });

    }



    public void removeReminder(ActionEvent event) throws IOException {

        remindersWarningLabel.setText(""); // remove the warning message in case it is there

        //remove the item from the list itself
        int index = remindersListView.getSelectionModel().getSelectedIndex();

        if (!observableListReminders.isEmpty() ) {
            observableListReminders.remove(index);
        } else {
            remindersWarningLabel.setText(" List is already empty. ");
        }

        //remove the item from the JSON file so that it does not show up again when the page is refreshed

        /*First, We read whatever is already in the json file.
         * We store that in an array. We convert the array into an ArrayList to make element removal easier.
         * Then we convert the ArrayList back into a normal array and overwrite the JSON file with the new list of reminders.
         * I had to use this solution in order to avoid running into JSON formatting issues as that would have taken more time, effort, and more code!*/
        Gson gson = new Gson();
        FileReader fileReader = new FileReader("src/main/resources/json/reminders.JSON");

        ArrayList<WioNotification.Reminder> notificationsList = new ArrayList<>();

        WioNotification.Reminder[] notifications = gson.fromJson(fileReader, WioNotification.Reminder[].class);
        notificationsList.addAll(Arrays.asList(notifications)); //get the array as an ArrayList
        if (notificationsList.size() > 0) {
            notificationsList.remove(index);//remove the selected element
        }
        notifications = notificationsList.toArray(new WioNotification.Reminder[0]);//convert the ArrayList into array again

        observableListReminders = FXCollections.observableArrayList(notifications); //overwrite the old observable list with a new one where the selected element is removed

        //write the new json to the file and save it.
        String json =  gson.toJson(observableListReminders);
        writeJsonToFile("src/main/resources/json/reminders.JSON", json);
        remindersListView.setItems(observableListReminders);

        MqttConnection publish = new MqttConnection(REMINDER);
        publish.publishPayload(REMINDER, json);


    }




    /*End----------------------------------------------------------Reminders screen------------------------------------------------------------------------*/



    /*Start--------------------------------------------------------status screen------------------------------------------------------------------------*/

    @FXML
    Label warningTextTemp;
    @FXML
    public void updateTemp() {


        MqttConnection connectionFar = new MqttConnection(TEMP_F);
        connectionFar.getPayload(TEMP_F,tempLabelFar,"F");

        MqttConnection connectionTemp = new MqttConnection(TEMP_C);
        connectionTemp.getPayload(TEMP_C,tempLabelCel,"C",warningTextTemp);


    }
    @FXML
    public void updateHumidity(){

        MqttConnection connectionHumi = new MqttConnection(HUMI);
        connectionHumi.getPayload(HUMI, humidityLabel,"%",warningText);
    }

    @FXML
    public void checkBathroomAvailability() {
        Image sadEmo = new Image("sadEmo.png");
        Image happyEmo = new Image("happyEmo.png");

        MqttConnection bathroomAvailability = new MqttConnection(MOTION_TOPIC);
        bathroomAvailability.getPayload(MOTION_TOPIC, bathroomAvailable,emoji,sadEmo,happyEmo);
    }

    /*End--------------------------------------------------------status screen------------------------------------------------------------------------*/



}


