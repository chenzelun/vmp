package com.chend.testapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String testStr = "hello world";
                try {
                    File filesDir = getFilesDir();
                    File test = new File(filesDir.getAbsolutePath() + "/test.txt");
                    FileWriter writer = new FileWriter(test);
                    writer.write(testStr);
                    writer.flush();
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                System.out.println("str: " + testStr);
                System.out.println("len: " + testStr.length());

                char[] buf = new char[1024];
                int count = -1;
                try {
                    File filesDir = getFilesDir();
                    File test = new File(filesDir.getAbsolutePath() + "/test.txt");
                    FileReader reader = new FileReader(test);
                    count = reader.read(buf);
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                buf[count] = '\0';
                System.out.println("str: " + Arrays.toString(buf));
                System.out.println("len: " + count);

                Intent intent = new Intent(MainActivity.this, Main2Activity.class);
                startActivity(intent);
            }
        });

        byte[] data = null;
        for (int i = 0; i < 50; i++) {
            data = new byte[10 * 1024 * 1024];
            System.out.println(" i: " + i);
        }
        System.out.println("2048: " + data[2048]);

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

}
