package com.example.myapplication;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;// for View
import android.widget.TextView;//for TextView
import android.widget.EditText;//for EditView
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    int size=30;
    public void bigger(View v){     // bigger按钮对应操作 button是对view的响应
        TextView txv= (TextView) findViewById(R.id.text1);  // 根据ID找到对应的text对象
        txv.setTextSize(++size);       // 修改对象的字符大小-size
    }
    public void smaller(View v){     // smaller按钮对应操作
        TextView txv;
        txv= (TextView) findViewById(R.id.text1);  // 根据ID找到对应的text对象
        txv.setTextSize(--size);       // 修改对象的字符大小-size
    }
    public void change(View v)  //change对应响应
    {
            EditText t1=(EditText)findViewById(R.id.editText);
            TextView t2=(TextView)findViewById(R.id.text1);
            t2.setText(t1.getText().toString());
    }
}
