package com.example.gr;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button mBtnDemoImageProcessing = findViewById(R.id.button_demo_image_processing);
        mBtnDemoImageProcessing.setOnClickListener(this);

        Button mBtnDemoService = findViewById(R.id.button_demo_ux);
        mBtnDemoService.setOnClickListener(this);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

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
