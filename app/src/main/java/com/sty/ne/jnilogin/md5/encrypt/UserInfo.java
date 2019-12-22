package com.sty.ne.jnilogin.md5.encrypt;

public class UserInfo {
    private String userName;
    private String userPwd;

    public UserInfo(String userName, String userPwd) {
        this.userName = userName;
        this.userPwd = userPwd;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getUserPwd() {
        return userPwd;
    }

    public void setUserPwd(String userPwd) {
        this.userPwd = userPwd;
    }

    /**
     * 在JNI中对密码加密
     */
    public native void nativeEncrypt();
}
