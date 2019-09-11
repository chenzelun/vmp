package com.chend.vmdemo;

import java.io.FileNotFoundException;

public class Dog extends Animal implements Land {
    private static final String TAG = Dog.class.getCanonicalName();
    private int no = 0;

    public Dog(int no){
        super();
        super.setNo(no);
        this.no = no;
        System.out.println(TAG + " " + no + " init.");
    }

    @Override
    public void walk() {
        System.out.println(TAG+ " " + no+" walk.");
    }

    public void drink(){
        System.out.println(TAG+ " " + no+" drink.");
    }

    public void callSuperDrink(){
        super.drink();
    }

    public void throwFileNo() throws FileNotFoundException {
        throw new FileNotFoundException();
    }
}
