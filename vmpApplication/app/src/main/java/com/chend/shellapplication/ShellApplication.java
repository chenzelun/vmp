package com.chend.shellapplication;

import android.annotation.SuppressLint;
import android.app.Application;

public class ShellApplication extends Application {
    @SuppressLint("MissingSuperCall")
    @Override
    public void onCreate() {
        System.loadLibrary("shell");
    }
}
