package com.example.peekapp;

import com.hivemq.client.mqtt.mqtt5.Mqtt5BlockingClient;
import javafx.application.Platform;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import static com.hivemq.client.mqtt.MqttGlobalPublishFilter.ALL;
import static java.nio.charset.StandardCharsets.UTF_8;

public class MqttConnection {

    private final String TEMP_C = "bathroom/celsius";
    private final String TEMP_F = "bathroom/fahrenheit";
    private final String HUMI = "bathroom/humi";
    private final String MOTION_TOPIC = "bathroom/motion";
    private final String REMINDER = "reminder";

    private final String host = "46a88a13823b400e938c77fe57c6a159.s1.eu.hivemq.cloud";
    private final String username = "peekapp";
    private final String password = "Peekapp@1";
    private String topic;
    private Mqtt5BlockingClient client;

    //HiveMQ tutorial:
    //https://console.hivemq.cloud/clients/java-hivemq?uuid=638e597d0527493ba56a670b993e0b54

    public MqttConnection(String topic) {
        client = com.hivemq.client.mqtt.MqttClient.builder().useMqttVersion5().serverHost(host).serverPort(8883).
                sslWithDefaultConfig().buildBlocking();
        // connect to HiveMQ Cloud with TLS and username/pw
        client.connectWith().simpleAuth().username(username).password(UTF_8.encode(password)).applySimpleAuth().send();
        // subscribe to the topic "my/test/topic"
        client.subscribeWith().topicFilter(topic).send();

    }
    //host, username, and password shall not be accessible by other users this is why we don't have getters for those.

    public String getTopic() {
        return this.topic;
    }
    public void setTopic(String topic) {
        this.topic = topic;
    }
    public Mqtt5BlockingClient getClient() {
        return client;
    }
    public void setClient(Mqtt5BlockingClient client) {
        this.client = client;
    }


    public void publishPayload(String topic, String message){
        client.publishWith().topic(topic).payload(UTF_8.encode(message)).send();
    }


    public void getPayload(String topic, Label label, String unit,Label warningText) {
        MqttConnection establishConnection = new MqttConnection(topic);
        establishConnection.getClient().toAsync().publishes(ALL, publish -> {
            String payload = ("" + UTF_8.decode(publish.getPayload().get()));
            // Updating the labels is happening on a separate thread.
            Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    setLabelText(payload.substring(0, 4) + unit,label);
                    if (topic.equals(TEMP_C)) {
                        establishConnection.notify(payload.substring(0, 4), "temperature", 14, 30, unit, warningText);
                    } else if (topic.equals(HUMI)) {
                        establishConnection.notify(payload, "humidity", 30, 60, unit, warningText);
                    }
                }
            });
        });
    }
    public void getPayload(String topic, Label label, String unit) {
        MqttConnection establishConnection = new MqttConnection(topic);
        establishConnection.getClient().toAsync().publishes(ALL, publish -> {
            String payload = ("" + UTF_8.decode(publish.getPayload().get()));
            // Updating the labels is happening on a separate thread.
            Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    setLabelText(payload.substring(0, 4) + unit,label);
                }
            });
        });
    }

    public void getPayload(String topic, Label label, ImageView emoji, Image sadEmo, Image happyEmo) {
        MqttConnection establishConnection = new MqttConnection(topic);
        establishConnection.getClient().toAsync().publishes(ALL, publish -> {
            String payload = ("" + UTF_8.decode(publish.getPayload().get()));
            // Updating the labels is happening on a separate thread.
            Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    if (payload.equals("occupied")){
                        setLabelText("Bathroom is currently " +payload+".",label);
                        setImageView(emoji,sadEmo);
                    } else {
                        setLabelText("Bathroom is currently " + payload + ".",label);
                        setImageView(emoji,happyEmo);
                    }
                }
            });
        });
    }

    public void notify (String payload,String measure,int min, int max, String unit, Label warningText){
        Double payloadDouble = Double.parseDouble(payload);

        if(payloadDouble<min) {
            setLabelText("The "+measure+ " is under "+ min+unit+".", warningText);

        }else if(payloadDouble>max){
            setLabelText("The "+measure+ " is over "+ max+unit+".",warningText);

        }else {
            setLabelText("",warningText);

        }
    }

    public void setLabelText (String text,Label label){
        if (label != null){
            label.setText(text);
        }
    }
    public void setImageView (ImageView view, Image image){
        if(view != null){
            view.setImage(image);
        }
    }
}


