package com.chend.vmdemo;

public abstract class Animal {
    private static final String TAG = Animal.class.getCanonicalName();

    public int getNo() {
        System.out.println(TAG+ " " + no+ "getNo.");
        return no;
    }

    public void setNo(int no) {
        this.no = no;
        System.out.println(TAG+ " " + no+ "getNo.");
    }

    public Animal(){
        System.out.println(TAG + " " + no+ " init.");
    }

    private int no;

    public final void say(){
        System.out.println(TAG+ " " + no + " say.");
    }

    public void drink(){
        System.out.println(TAG+ " " + no+" drink.");
    }

    public void throwNullExp(){
        System.out.println(TAG+ " " + no+ "throwNullExp.");
        Land land = null;
        land.walk();
        System.out.println(TAG+ " " + no+ "throwNullExp222222.");
    }
}
