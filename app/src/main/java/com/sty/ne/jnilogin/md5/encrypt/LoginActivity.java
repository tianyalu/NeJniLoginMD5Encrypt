package com.sty.ne.jnilogin.md5.encrypt;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class LoginActivity extends AppCompatActivity {
    private EditText edtUser;
    private EditText edtPwd;
    private Button btnLogin;

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        initView();
    }

    private void initView() {
        edtUser = findViewById(R.id.edt_user);
        edtPwd = findViewById(R.id.edt_pwd);
        btnLogin = findViewById(R.id.btn_login);

        btnLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                login();
            }
        });
    }

    private void login() {
        String strUser = edtUser.getText().toString().trim();
        String strPwd = edtPwd.getText().toString();
        if(TextUtils.isEmpty(strUser) || TextUtils.isEmpty(strPwd)) {
            Toast.makeText(this, "用户名或密码为空", Toast.LENGTH_SHORT).show();
        } else {
            UserInfo userInfo = new UserInfo(strUser, strPwd);
            userInfo.nativeEncrypt();
            Toast.makeText(this, "密码: " + userInfo.getUserPwd(), Toast.LENGTH_SHORT).show();

            doMockLogin(strUser, userInfo.getUserPwd());
            saveUserAndPwd(this, strUser, userInfo.getUserPwd());

//            doMockLogin(strUser, MD5Helper.getMD5(strPwd));
//            saveUserAndPwd(this, strUser, MD5Helper.getMD5(strPwd));
        }
    }

    private void saveUserAndPwd(Context context, String strUser, String pwd) {
        //todo 保存用户名和密码
    }

    private void doMockLogin(String userName, String pwd) {
        //todo 登录
    }
}
