package fi.haippirinki.l2p;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

public class Platform {
    private static SharedPreferences preferences = null;

    static void init(Activity activity) {
        preferences = activity.getPreferences(Context.MODE_PRIVATE);
    }

    static void deinit() {
        preferences = null;
    }

    public static boolean getBooleanPreference(String key) {
        return preferences.getBoolean(key, false);
    }

    public static float getFloatPreference(String key) {
        return preferences.getFloat(key, 0.f);
    }

    public static int getIntPreference(String key) {
        return preferences.getInt(key, 0);
    }

    public static long getLongPreference(String key) {
        return preferences.getLong(key, 0);
    }

    public static String getStringPreference(String key) {
        return preferences.getString(key, "");
    }

    public static void putBooleanPreference(String key, boolean value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    public static void putFloatPreference(String key, float value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putFloat(key, value);
        editor.apply();
    }

    public static void putIntPreference(String key, int value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putInt(key, value);
        editor.apply();
    }

    public static void putLongPreference(String key, long value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putLong(key, value);
        editor.apply();
    }

    public static void putStringPreference(String key, String value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(key, value);
        editor.apply();
    }
}
