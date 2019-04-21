package com.example.gr;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.util.ArrayList;

public class DemoVideoActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "mytag-DemoVideoActivity";
    private ImageView mImgFramePreview;
    private TextView mTxtIntro;
    private ArrayList<File> mFileList = new ArrayList<>();
    private int mCurrentFrame = 0;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");

                    // Load native library after(!) OpenCV initialization
                    System.loadLibrary("native-lib");
                    if (mCurrentFrame < mFileList.size()) processFrame();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_demo_video);

        mTxtIntro = findViewById(R.id.text_view_intro);

        mImgFramePreview = findViewById(R.id.image_view_frame);
        mImgFramePreview.setOnClickListener(this);

        File root = new File(Environment.getExternalStorageDirectory().getPath() + "/gr/");
        getFile(root);
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

    private void getFile(File dir) {
        File listFile[] = dir.listFiles();
        if (listFile != null && listFile.length > 0) {
            for (int i = 0; i < listFile.length; i++) {
                if (listFile[i].getName().endsWith(".png")
                        || listFile[i].getName().endsWith(".jpg")
                        || listFile[i].getName().endsWith(".jpeg")) {
                    String temp = listFile[i].getPath().substring(0, listFile[i].getPath().lastIndexOf('/'));
                    mFileList.add(listFile[i]);
                }
            }
        } else {
            Log.d(TAG, "!(listFile != null && listFile.length > 0)");
        }
    }

    void processFrame() {
        Mat matRgb = new Mat();
        Mat matGray = new Mat();
        String stringFile = mFileList.get(mCurrentFrame).getName();
        Log.d(TAG, "getFile: " + stringFile);

        Bitmap bitmapRbg = BitmapFactory.decodeFile(mFileList.get(mCurrentFrame).getAbsolutePath());
        Utils.bitmapToMat(bitmapRbg, matRgb);
        bitmapRbg.recycle();

        Imgproc.cvtColor(matRgb, matGray, Imgproc.COLOR_RGB2GRAY);


        int[] result = detectCrosswalk(matGray.getNativeObjAddr());
        matGray.release();

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

        for (int i = result[1]; i < result[0]; i += 4) {
            Point pt1 = new Point(result[i], result[i + 1]);
            Point pt2 = new Point(result[i + 2], result[i + 3]);
            Imgproc.line(matRgb, pt1, pt2, new Scalar(0, 0, 255), 8);
        }
        if (result[2] == 1) { // draw valid line
            for (int i = result[0] - 3 * 4; i < result[0]; i += 4) {
                Point pt1 = new Point(result[i], result[i + 1]);
                Point pt2 = new Point(result[i + 2], result[i + 3]);
                Imgproc.line(matRgb, pt1, pt2, new Scalar(255, 0, 0), 8);
            }
        }

        Mat matRgbPortrait = new Mat(matRgb.cols(), matRgb.rows(), CvType.CV_8UC3);
        Core.rotate(matRgb, matRgbPortrait, Core.ROTATE_90_CLOCKWISE);
        matRgb.release();

        Bitmap bitmapRbgPortrait = Bitmap.createBitmap(matRgbPortrait.width(), matRgbPortrait.height(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(matRgbPortrait, bitmapRbgPortrait);
        matRgbPortrait.release();
        mImgFramePreview.setImageBitmap(bitmapRbgPortrait);
        mCurrentFrame++;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.image_view_frame && mCurrentFrame < mFileList.size()) {
            Log.d(TAG, "onClick: ");
            processFrame();
            mTxtIntro.setVisibility(View.GONE);
        }
    }

    public native int[] detectCrosswalk(long matAddrGr);

}
