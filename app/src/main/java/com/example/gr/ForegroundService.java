package com.example.gr;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.widget.Toast;

import static com.example.gr.App.CHANNEL_ID;

public class ForegroundService extends Service {
    private static final String TAG = "mytag-ForegroundService";
    private static final int ONGOING_NOTIFICATION_ID = 1566;

    private RotationVectorSensor mRotationVectorSensor;
    private ChatHead mChatHead;

    @Override
    public void onCreate() {
        Toast.makeText(this, "Service Created", Toast.LENGTH_LONG).show();
        mChatHead = new ChatHead(this);

        mRotationVectorSensor = new RotationVectorSensor(this);
        mRotationVectorSensor.start();
    }

    ChatHead getChatHead() {
        return mChatHead;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
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

    @Override
    public void onDestroy() {
        Toast.makeText(this, "Service Stopped", Toast.LENGTH_LONG).show();
        mRotationVectorSensor.stop();
        mChatHead.remove();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
