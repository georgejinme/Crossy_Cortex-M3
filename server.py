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

def scoreQueryFunc():
    print "正在执行查询【成绩】..."
    result = requests.get("http://electsys.sjtu.edu.cn/edu/login.aspx")
    sessionID = result.cookies.get("ASP.NET_SessionId", "none")
    if sessionID == "none":
        print "Session未保存，请重新登录"


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
