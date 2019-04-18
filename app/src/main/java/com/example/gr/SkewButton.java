package com.example.gr;

import android.content.Context;
import android.graphics.Canvas;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

public class SkewButton extends android.support.v7.widget.AppCompatButton {

    private static final String TAG = "mytag-SkewButton";
    public static float skewX = 1.0f, skewY = 1.0f;
    public static float fromDegrees = -20, toDegrees = 30, centerX = 0, centerY = 0, depthZ = 0;
    public static boolean reverse = false;

    Context mContext;

    public SkewButton(Context context) {
        super(context);
        mContext = context;
    }

    public SkewButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
    }

    public SkewButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        // TODO Auto-generated method stub
        super.onDraw(canvas);
        canvas.skew(skewX, skewY);
        Rotate3dAnimation skew = new Rotate3dAnimation(
                fromDegrees, toDegrees, centerX, centerY, depthZ, reverse);
        startAnimation(skew);
        Log.d(TAG, "onDraw: skewX: " + skewX +
                ", skewY: " + skewY +
                ", fromDegrees: " + fromDegrees +
                ", toDegrees: " + toDegrees +
                ", centerX: " + centerX +
                ", centerY: " + centerY);
    }
}
