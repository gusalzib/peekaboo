package com.example.peekapp;
import com.hivemq.client.mqtt.MqttClient;
import com.hivemq.client.mqtt.mqtt5.Mqtt5BlockingClient;


import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;

import static com.hivemq.client.mqtt.MqttGlobalPublishFilter.ALL;
import static java.nio.charset.StandardCharsets.UTF_8;

//JAR file from https://jar-download.com/download-handling.php
public class MainTest {

        public static void main(String[] args) throws Exception {
            // Define the absolute paths
            Path source = Paths.get("C:/Users/arnao/Desktop/University Software Engineering/miniProject2/peekaboo/pe-kaboo/app-code/peekApp/src/main/resources/com/example/peekapp/reminders.JSON");
            Path current = Paths.get("C:/Users/arnao/Desktop/University Software Engineering/miniProject2/peekaboo/pe-kaboo/app-code/peekApp/src/main/java/com/example/peekapp/Controller.java");

            // Find the common parent directory
            File commonParent = findCommonParent(source.toFile(), current.toFile());

            // Calculate the relative paths from the common parent directory
            String relativeSource = getRelativePath(commonParent, source.toFile());
            String relativeCurrent = getRelativePath(commonParent, current.toFile());

            // Combine the relative paths to get the final relative path
            String relativePath = relativeCurrent + File.separator + relativeSource;

            // Print the relative path
            System.out.println(relativePath);



            final String host = "638e597d0527493ba56a670b993e0b54.s1.eu.hivemq.cloud";
            final String username = "peekapp";
            final String password = "Peekapp@1";

            // create an MQTT client
            //https://javadoc.io/static/com.hivemq/hivemq-mqtt-client/1.2.2/com/hivemq/client/mqtt/MqttClientBuilder.html
            // https://www.oracle.com/corporate/features/simple-messaging-with-mqtt.html
            //https://console.hivemq.cloud/clients/java-hivemq
            final Mqtt5BlockingClient client = MqttClient.builder()
                    .useMqttVersion5()
                    .serverHost(host)
                    .serverPort(8883)
                    .sslWithDefaultConfig()
                    .buildBlocking();

            // connect to HiveMQ Cloud with TLS and username/pw
            client.connectWith()
                    .simpleAuth()
                    .username(username)
                    .password(UTF_8.encode(password))
                    .applySimpleAuth()
                    .send();

            System.out.println("Connected successfully");

            // subscribe to the topic "my/test/topic"
            client.subscribeWith()
                    .topicFilter("bathroom/temp")
                    .send();

            // set a callback that is called when a message is received (using the async API style)

            client.toAsync().publishes(ALL, publish -> {
                System.out.println("Received message: " +
                        publish.getTopic() + " -> " +
                        UTF_8.decode(publish.getPayload().get()));

                // disconnect the client after a message was received
                //client.disconnect();
            });


            // publish a message to the topic "my/test/topic"
//            client.publishWith()
//                    .topic("my/test/topic")
//                    .payload(UTF_8.encode("Hello"))
//                    .send();
        }
    private static File findCommonParent(File file1, File file2) {
        while (file1 != null && !file2.getAbsolutePath().startsWith(file1.getAbsolutePath())) {
            file1 = file1.getParentFile();
        }
        return file1;
    }

    // Helper method to get the relative path from a common parent directory
    private static String getRelativePath(File parent, File file) {
        String relativePath = file.getAbsolutePath().substring(parent.getAbsolutePath().length());
        if (relativePath.startsWith(File.separator)) {
            relativePath = relativePath.substring(1);
        }
        return relativePath;
    }
    }

