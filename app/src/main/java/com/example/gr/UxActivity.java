package com.example.gr;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

public class UxActivity extends AppCompatActivity {
    private RotationVectorSensor mRotationVectorSensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ux);

        mRotationVectorSensor = new RotationVectorSensor(this);
    }

    public void startService(View v) {
        startService(new Intent(v.getContext(), ForegroundService.class));
    }

    public void stopService(View v) {
        stopService(new Intent(v.getContext(), ForegroundService.class));
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
