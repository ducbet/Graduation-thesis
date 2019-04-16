package com.example.gr;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class ScreenOnOffReceiver extends BroadcastReceiver {
    private static final String TAG = "mytag-ScreenOnOff";
    private static final String REASON_SCREEN_OFF = "SCREEN_OFF";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction() == null) return;

        boolean screenOn = true;
        if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF)) {
            Log.d(TAG, "onReceive: ACTION_SCREEN_OFF");
            screenOn = false;
        } else if (intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {
            Log.d(TAG, "onReceive: ACTION_USER_PRESENT");
            screenOn = true;
        }

        if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF) || intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {
            Intent i = new Intent(context, ForegroundService.class);
            i.putExtra("reason_changed", REASON_SCREEN_OFF);
            i.putExtra("new_state", screenOn);
            context.startService(i);
        }
    }
}
