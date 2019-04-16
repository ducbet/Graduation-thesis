package com.example.gr;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.widget.Toast;

import static com.example.gr.App.CHANNEL_ID;

public class ForegroundService extends Service {
    private static final String TAG = "mytag-ForegroundService";
    private static final int ONGOING_NOTIFICATION_ID = 1566;
    private final BroadcastReceiver mScreenOnOffReceiver = new ScreenOnOffReceiver();

    private RotationVectorSensor mRotationVectorSensor;
    private ChatHead mChatHead;

    @Override
    public void onCreate() {
        Toast.makeText(this, "Service Created", Toast.LENGTH_LONG).show();
        mChatHead = new ChatHead(this);

        mRotationVectorSensor = new RotationVectorSensor(this);

        final IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        registerReceiver(mScreenOnOffReceiver, filter);
    }

    public void onResume() {
        Toast.makeText(this, "Service Resumed", Toast.LENGTH_LONG).show();
        Log.d(TAG, "Service Resumed: ");
        mRotationVectorSensor.start();
        mChatHead.show();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Toast.makeText(this, "Service onStartCommand", Toast.LENGTH_LONG).show();
        Log.d(TAG, "onStartCommand: ");
        String reasonChanged = intent.getStringExtra("reason_changed");
        boolean newState = intent.getBooleanExtra(reasonChanged, true);

        if (newState) onResume();
        else onPause();

        Intent notificationIntent = new Intent(this, UxActivity.class);
        PendingIntent pendingIntent =
                PendingIntent.getActivity(this, 0, notificationIntent, 0);

        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("GR APP")
                .setContentText("detecting crosswalk")
                .setSmallIcon(R.drawable.ic_zebra)
                .setContentIntent(pendingIntent)
                .build();

        startForeground(ONGOING_NOTIFICATION_ID, notification);

        return START_STICKY;
    }

    public void onPause() {
        Toast.makeText(this, "Service Paused", Toast.LENGTH_LONG).show();
        Log.d(TAG, "Service Paused: ");
        mRotationVectorSensor.stop();
        mChatHead.hide();
    }

    @Override
    public void onDestroy() {
        Toast.makeText(this, "Service Stopped", Toast.LENGTH_LONG).show();
        mRotationVectorSensor.stop();
        mChatHead.remove();
        unregisterReceiver(mScreenOnOffReceiver);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    ChatHead getChatHead() {
        return mChatHead;
    }
}
