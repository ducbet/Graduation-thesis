package com.example.gr;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

public class UxActivity extends AppCompatActivity {
    private RotationVectorSensor mRotationVectorSensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ux);

        mRotationVectorSensor = new RotationVectorSensor(this);


    }

    @Override
    protected void onResume() {
        super.onResume();
        mRotationVectorSensor.start();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mRotationVectorSensor.stop();
    }
}
