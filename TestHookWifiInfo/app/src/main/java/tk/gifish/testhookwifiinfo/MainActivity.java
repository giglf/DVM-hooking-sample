package tk.gifish.testhookwifiinfo;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



    }

    public void onClick(View view) {

        TextView textView = findViewById(R.id.textView);

        WifiManager manager = (WifiManager)getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        WifiInfo info = manager.getConnectionInfo();

        textView.setText(info.getMacAddress());

    }
}
