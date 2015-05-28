#-*- coding:gbk -*-
#-------------------------------------------------------------------------------
# Name:        ??1
# Purpose:
#
# Author:      lenovo
#
# Created:     27/05/2015
# Copyright:   (c) lenovo 2015
# Licence:     <your licence>
#-------------------------------------------------------------------------------
import requests
import json
import os
import webbrowser

crossyWebPrefix = "http://tq5124.dy.tongqu.me/crossy"

def getScoreCookie(url):
    result = requests.get(url)
    sessionID = result.cookies.get("ASP.NET_SessionId", "none")
    return sessionID

def scoreQueryFunc():
    print "正在执行查询【成绩】..."

    sessionID = getScoreCookie("http://electsys.sjtu.edu.cn/edu/student/sdtMain.aspx")
    print sessionID
    postData = {'cookie':'ASP.NET_SessionId=' + sessionID}
    result = requests.post(crossyWebPrefix + "/api/1/elect/info", params=postData)
    print result.text

    '''if result.status_code == 400:
        print "Session未保存，请重新登录"
        os.system("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe")
        webbrowser.open("http://electsys.sjtu.edu.cn/edu/login.aspx")
    print("完成登录时请输入\"login complete\"")

    while (True):
        a = raw_input()
        if a == "login complete":
            break

    sessionID = getScoreCookie("http://electsys.sjtu.edu.cn/edu/student/sdtMain.aspx")
    if sessionID != "none":
        postData = {'cookie':'ASP.NET_SessionId=' + sessionID}
        result = requests.post(crossyWebPrefix + "/api/1/elect/info", params=postData)
        print result.text'''



def booksQueryFunc():
    print "正在执行查询【书籍】..."
def busQueryFunc():
    print "正在执行查询【校车时刻表】..."
def ecardQueryFunc():
    print "正在执行查询【校园卡信息】..."

def main():
    print "---------Crossy Server---------"
    queryType = "scoreQuery"

    if queryType == "scoreQuery":
        scoreQueryFunc()
    elif queryType == "booksQuery":
        booksQueryFunc()
    elif queryType == "busQuery":
        busQueryFunc()
    elif queryType == "ecardQuery":
        ecardQueryFunc();
    else: print "Unavailable Type. Please Check."


if __name__ == '__main__':
    main()
