package com.chend.vmdemo;

import android.os.Bundle;
import android.os.HandlerThread;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import java.io.FileNotFoundException;
import java.lang.reflect.Field;


public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getCanonicalName();
    private Animal[] dogs = null;
    private float[] num0;
    private char[] num1;
    private static byte[] num2;
    private int[] data;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        try {
//            Class clazz = this.getClass();
//            Field dogsF = clazz.getDeclaredField("dogs");
//            dogsF.set(null, new Dog[10]);
//            if(MainActivity.dogs!=null){
//                System.out.println(MainActivity.dogs.length);
//            }
//        } catch (NoSuchFieldException e) {
//            e.printStackTrace();
//        } catch (IllegalAccessException e) {
//            e.printStackTrace();
//        }

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);

        tv.setText(" " + test1(20, 0));



    }

    public long test1(float a, double b){
        this.dogs = new Dog[10];
        for(int i=0; i<10; i++){
            this.dogs[i] = new Dog(i);
        }

        if(this.dogs[3]!=null && this.dogs[3] instanceof Dog){
            this.dogs[3].drink();
        }

        data = new int[26];
        for(int i=0; i<26; i++){
            data[i] = i;
        }

        num0 = new float[10];
        num1 = new char[10];
        num2 = new byte[10];
        for(int t: data){
            switch (t%3){
                case 0: num0[t/3]=t;break;
                case 1: num1[t/3]= (char) t;break;
                case 2: num2[t/3]= (byte) t;break;
                default:break;
            }
        }

        if (-num0[2]<5){
           System.out.println( super.getClass().getCanonicalName());
        }

        long d = num2[2];
        long e = ~d;
        System.out.println(e);

        return 255;
    }


}
