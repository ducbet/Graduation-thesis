package com.example.gr;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button mBtnDemoImageProcessing = findViewById(R.id.button_demo_image_processing);
        mBtnDemoImageProcessing.setOnClickListener(this);

        Button mBtnDemoService = findViewById(R.id.button_demo_ux);
        mBtnDemoService.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button_demo_image_processing:
                startActivity(new Intent(MainActivity.this, ImageProcessingActivity.class));
                break;
            case R.id.button_demo_ux:
                startActivity(new Intent(MainActivity.this, UxActivity.class));
                break;
        }
    }
}
