package com.example.gr;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.WindowManager;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

public class DemoCameraActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2 {
    private static final String TAG = "MYTAG-ImageProcessAct";

    private Mat mGray, mRgba;

    private CameraBridgeViewBase mOpenCvCameraView;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");

                    // Load native library after(!) OpenCV initialization
                    System.loadLibrary("native-lib");
                    mOpenCvCameraView.enableView();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    /**
     * Called when the activity is first created.
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_demo_camera);

        mOpenCvCameraView = findViewById(R.id.java_camera_surface_view);
        mOpenCvCameraView.setVisibility(CameraBridgeViewBase.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }


    @Override
    public void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mGray = new Mat(height, width, CvType.CV_8UC1);
        mRgba = new Mat(height, width, CvType.CV_8UC4);
    }

    @Override
    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mGray = inputFrame.gray();
        mRgba = inputFrame.rgba();
        int[] result = detectCrosswalk(mGray.getNativeObjAddr());
        Log.d(TAG, "onCameraFrame: " +
                "\nresult[0]: " + result[0] +
                ", \nresult[1]: " + result[1] +
                ", \nresult[2]: " + result[2]);
        if (result[0] > 3) {
            Log.d(TAG, "onCameraFrame: " +
                    "\nresult[3]: " + result[3] +
                    ", \nresult[4]: " + result[4] +
                    ", \nresult[5]: " + result[5] +
                    ", \nresult[6]: " + result[6]);
        }
        Log.d(TAG, "onCameraFrame: width: " + mGray.width() + ", height: " + mGray.height() + "\n.\n.\n");
        for (int i = result[1]; i < result[0]; i += 4) {
            Point pt1 = new Point(result[i], result[i + 1]);
            Point pt2 = new Point(result[i + 2], result[i + 3]);
            Imgproc.line(mRgba, pt1, pt2, new Scalar(0, 0, 255), 3);
        }
        return mRgba;
    }

    public native int[] detectCrosswalk(long matAddrGr);
}
