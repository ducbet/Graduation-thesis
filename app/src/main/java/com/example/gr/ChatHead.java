package com.example.gr;

import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.Handler;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;

import java.util.Random;

import static android.content.Context.WINDOW_SERVICE;

public class ChatHead extends View implements View.OnTouchListener {

    private Context mContext;

    private WindowManager mWindowManager;
    private WindowManager.LayoutParams mChatHeadParams;
    private View mChatHeadView;
    private ImageView mChatHeadImage;
    private LinearLayout mUntouchableOverlap;

    // View.OnTouchListener
    private int lastAction;
    private int initialX;
    private int initialY;
    private float initialTouchX;
    private float initialTouchY;

    public ChatHead(Context context) {
        super(context);
        mContext = context;
        init();
    }

    private void init() {
        mWindowManager = (WindowManager) mContext.getSystemService(WINDOW_SERVICE);
        createOverlay();
        createChatHead();
    }

    private void createOverlay() {//Inflate the chat head layout we created
        View mOverlayView = LayoutInflater.from(mContext).inflate(R.layout.layout_overlay, null);
        //Add the view to the window.
        WindowManager.LayoutParams mOverlayParams = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);

        //Specify the chat head position
        mOverlayParams.gravity = Gravity.TOP | Gravity.START;        //Initially view will be added to top-left corner
        mOverlayParams.x = 0;
        mOverlayParams.y = 0;

        //Add the view to the window
        mWindowManager.addView(mOverlayView, mOverlayParams);
        mUntouchableOverlap = mOverlayView.findViewById(R.id.linear_layout_overlap);
    }

    private void createChatHead() {
        //Inflate the chat head layout we created
        mChatHeadView = LayoutInflater.from(mContext).inflate(R.layout.layout_chat_head, null);
        //Add the view to the window.
        mChatHeadParams = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);

        //Specify the chat head position
        mChatHeadParams.gravity = Gravity.TOP | Gravity.START;        //Initially view will be added to top-left corner
        mChatHeadParams.x = 0;
        mChatHeadParams.y = 100;

        //Add the view to the window
        mWindowManager.addView(mChatHeadView, mChatHeadParams);

        //Set the close button.
        ImageView mCloseButton = mChatHeadView.findViewById(R.id.image_view_close_chat_head);
        mCloseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //close the service and remove the chat head from the window
                mContext.stopService(new Intent(mContext, ForegroundService.class));
            }
        });
        //Drag and move chat head using user's touch action.
        mChatHeadImage = mChatHeadView.findViewById(R.id.image_view_chat_head);
        mChatHeadImage.setOnTouchListener(this);
    }

    @Override
    public boolean performClick() {
        super.performClick();
        if (mUntouchableOverlap.getVisibility() == View.VISIBLE) {
            mChatHeadImage.setImageResource(R.drawable.ic_android_circle);
            mUntouchableOverlap.setVisibility(View.GONE);
        } else {
            mChatHeadImage.setImageResource(R.drawable.ic_android_circle_red);
            mUntouchableOverlap.setVisibility(View.VISIBLE);
        }
        return true;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                //remember the initial position.
                initialX = mChatHeadParams.x;
                initialY = mChatHeadParams.y;

                //get the touch location
                initialTouchX = event.getRawX();
                initialTouchY = event.getRawY();

                lastAction = event.getAction();
                return true;
            case MotionEvent.ACTION_UP:
                if (lastAction == MotionEvent.ACTION_DOWN && v.getId() == R.id.image_view_chat_head) {
                    performClick();
                }
                lastAction = event.getAction();
                return true;
            case MotionEvent.ACTION_MOVE:
                //Calculate the X and Y coordinates of the view.
                mChatHeadParams.x = initialX + (int) (event.getRawX() - initialTouchX);
                mChatHeadParams.y = initialY + (int) (event.getRawY() - initialTouchY);

                //Update the layout with new X & Y coordinate
                mWindowManager.updateViewLayout(mChatHeadView, mChatHeadParams);
                lastAction = event.getAction();
                return true;
        }
        return false;
    }

    public void remove() {
        if (mChatHeadView != null) mWindowManager.removeView(mChatHeadView);
    }
}
