package com.example.gr;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private Button mBtnDemoImageProcessing, mBtnDemoService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mBtnDemoImageProcessing = findViewById(R.id.button_demo_image_processing);
        mBtnDemoImageProcessing.setOnClickListener(this);

        mBtnDemoService = findViewById(R.id.button_demo_ux);
        mBtnDemoService.setOnClickListener(this);

        editSkewButtonParams();
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

    private void editSkewButtonParams() {
        Button btnRedraw = findViewById(R.id.button_redraw);
        final EditText edtSkewX = findViewById(R.id.edit_text_skewX);
        final EditText edtSkewY = findViewById(R.id.edit_text_skewY);
        final EditText edtFromDegrees = findViewById(R.id.edit_text_fromDegrees);
        final EditText edtToDegrees = findViewById(R.id.edit_text_toDegrees);
        final EditText edtCenterX = findViewById(R.id.edit_text_centerX);
        final EditText edtCenterY = findViewById(R.id.edit_text_centerY);
        btnRedraw.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SkewButton.skewX = Float.parseFloat(edtSkewX.getText().toString());
                SkewButton.skewY = Float.parseFloat(edtSkewY.getText().toString());
                SkewButton.fromDegrees = Float.parseFloat(edtFromDegrees.getText().toString());
                SkewButton.toDegrees = Float.parseFloat(edtToDegrees.getText().toString());
                SkewButton.centerX = Float.parseFloat(edtCenterX.getText().toString());
                SkewButton.centerY = Float.parseFloat(edtCenterY.getText().toString());
                mBtnDemoImageProcessing.invalidate();
            }
        });
    }
}
