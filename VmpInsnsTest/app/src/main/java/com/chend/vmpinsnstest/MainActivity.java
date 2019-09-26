package com.chend.vmpinsnstest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        testJNI();
    }

    public void test() {
        System.out.println("start");
        for (int i = 0; i < 20; i++) {
            System.out.println(" " + i + ": " + size(new double[(i + 1) * 1024][1024]));
        }
        System.out.println("ok");
    }

    public native void testJNI();

    public native int size(double[][] array);
}
