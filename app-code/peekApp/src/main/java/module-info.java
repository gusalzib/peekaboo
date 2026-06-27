module com.example.peekapp {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.web;

    requires org.controlsfx.controls;
    requires com.dlsc.formsfx;
    requires net.synedra.validatorfx;
    requires org.kordamp.ikonli.javafx;
    requires org.kordamp.bootstrapfx.core;

    requires com.almasb.fxgl.all;
    requires org.eclipse.paho.client.mqttv3;
    requires com.hivemq.client.mqtt;
    requires org.jetbrains.annotations;
    requires com.google.gson;

    opens com.example.peekapp to javafx.fxml;
    exports com.example.peekapp;
}