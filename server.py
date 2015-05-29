#-*- coding:utf-8 -*-
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
import sqlite3
import win32crypt

crossyWebPrefix = "http://tq5124.dy.tongqu.me/crossy"

def getLocalCookie():
    cookie_file_path = os.path.join(os.environ['LOCALAPPDATA'], r'Google\Chrome\User Data\Default\Cookies')
    conn = sqlite3.connect(cookie_file_path)
    sql = 'select host_key, name, value, encrypted_value from cookies where host_key like "%{}%"'.format("electsys.sjtu.edu.cn")
    for i in conn.execute(sql):
        if i[1] == "ASP.NET_SessionId":
            pwdHash = str(i[3])
            try:
                ret = win32crypt.CryptUnprotectData(pwdHash, None, None, None, 0)
            except:
                print 'Fail to decrypt chrome cookies'
                sys.exit(-1)
            with open("cookie.txt", 'w') as outFile:
                outFile.write(ret[1].encode('gbk'))
    conn.close()
    file = open("cookie.txt", 'r')
    return file.readline()


def scoreQueryFunc():
    print "Query Score..."

    sessionID = getLocalCookie()
    postData = {"cookie":"ASP.NET_SessionId=" + sessionID}
    result = requests.post(crossyWebPrefix + "/api/1/elect/info", postData)

    if result.status_code == 400:
        print result.error
        os.system("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe")
        webbrowser.open("http://electsys.sjtu.edu.cn/edu/login.aspx")

        print("Input \"login complete\" when you have logined")
        while (True):
            a = raw_input()
            if a == "login complete":
                break
        sessionID = getLocalCookie()
        if sessionID != "none":
            postData = {'cookie':'ASP.NET_SessionId=' + sessionID}
            print postData
            result = requests.post(crossyWebPrefix + "/api/1/elect/info", postData)

    print "Get Score Info, Waiting for minutes..."
    print result.text
    scoreData = json.loads(result.text, encoding='utf-8')
    print scoreData['info']['school']


def booksQueryFunc():
    print "Querying Books..."
def busQueryFunc():
    print "Querying Bus Schedule..."
def ecardQueryFunc():
    print "Querying E-Card Info..."

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
