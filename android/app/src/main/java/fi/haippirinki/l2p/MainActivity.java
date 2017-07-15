package fi.haippirinki.l2p;

import android.os.Build;
import android.view.View;

public class MainActivity extends android.app.NativeActivity {
	protected void onCreate(android.os.Bundle savedInstanceState) {
        Platform.init(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }

        super.onCreate(savedInstanceState);
    }

    protected void onDestroy() {
        super.onDestroy();
        Platform.deinit();
    }
}
