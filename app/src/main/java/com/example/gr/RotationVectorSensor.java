package com.example.gr;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.widget.TextView;

import static android.content.Context.SENSOR_SERVICE;

public class RotationVectorSensor implements SensorEventListener {

    private Context mContext;
    private SensorManager mSensorManager;
    private Sensor mRotationVectorSensor;
    private final float[] mRotationMatrix = new float[16];
    private final float[] mOrientations = new float[3];
    private long mLastSamplingMs = 0;

    private TextView mTxtDataRotv;

    public RotationVectorSensor(Context context) {
        mContext = context;
        // Get an instance of the SensorManager
        mSensorManager = (SensorManager) context.getSystemService(SENSOR_SERVICE);
        // find the rotation-vector sensor
        mRotationVectorSensor = mSensorManager.getDefaultSensor(
                Sensor.TYPE_ROTATION_VECTOR);
        // initialize the rotation matrix to identity
        mRotationMatrix[0] = 1;
        mRotationMatrix[4] = 1;
        mRotationMatrix[8] = 1;
        mRotationMatrix[12] = 1;

        mTxtDataRotv = ((Activity) mContext).findViewById(R.id.text_view_rotv);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        // we received a sensor event. it is a good practice to check
        // that we received the proper event
        if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
            // convert the rotation-vector to a 4x4 matrix. the matrix
            // is interpreted by Open GL as the inverse of the
            // rotation-vector, which is what we want.
            SensorManager.getRotationMatrixFromVector(mRotationMatrix, event.values);
            SensorManager.getOrientation(mRotationMatrix, mOrientations);

            for (int i = 0; i < 3; i++) {
                mOrientations[i] = (float) (Math.toDegrees(mOrientations[i]));
            }

            String strData = "orientations[0]: " + Math.round(mOrientations[0] * 100.0) / 100.0 +
                    "\norientations[1]: " + Math.round(mOrientations[1] * 100.0) / 100.0 +
                    "\norientations[2]: " + Math.round(mOrientations[2] * 100.0) / 100.0 +
                    "\nsampling frequency: " + (System.currentTimeMillis() - mLastSamplingMs);
            mLastSamplingMs = System.currentTimeMillis();
            mTxtDataRotv.setText(strData);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    public void start() {
        // enable our sensor when the activity is resumed, ask for
        // 10 ms updates.
        mSensorManager.registerListener(this, mRotationVectorSensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    public void stop() {
        // make sure to turn our sensor off when the activity is paused
        mSensorManager.unregisterListener(this);
    }
}
