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

#------------------------course information--------------------------
class courses:
    name = ""
    credit = 0
    score = 0
    def __init__(self, n, c, s):
        self.credit = 'credits: ' + c
        self.name= 'name: ' + n
        self.score = 'score: ' + s
    def printCourse(self):
        print self.name + ' - ' + self.credit + ' - ' + self.score
#--------------------------------------------------------------------

#------------------------------get cookies---------------------------
def getLocalCookie(url, key):
    cookie_file_path = os.path.join(os.environ['LOCALAPPDATA'], r'Google\Chrome\User Data\Default\Cookies')
    conn = sqlite3.connect(cookie_file_path)
    sql = 'select host_key, name, value, encrypted_value from cookies where host_key like "%{}%"'.format(url)
    for i in conn.execute(sql):
        if i[1] == key:
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
#--------------------------------------------------------------------

#-------------------------deal with score query----------------------
def dealWithScore(data):
    scoreInfo = {}
    for i in data:
        crs = courses(i['name'], i['credit'], i['score'])
        semester = i['year'] + '-' + i['semester']
        if scoreInfo.has_key(semester):
            scoreInfo[semester].append(crs)
        else:
            scoreInfo[semester] = [crs]
    return scoreInfo

def scoreQueryFunc():
    print "Query Score..."

    sessionID = getLocalCookie("electsys.sjtu.edu.cn", "ASP.NET_SessionId")
    postData = {"cookie":"ASP.NET_SessionId=" + sessionID}
    result = requests.post(crossyWebPrefix + "/api/1/elect/info", postData)

    if result.status_code == 400:
        print "please login"
        os.system("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe")
        webbrowser.open("http://electsys.sjtu.edu.cn/edu/login.aspx")
        print("Input \"login complete\" when you have logined")
        while (True):
            a = raw_input()
            if a == "login complete":
                break
        sessionID = getLocalCookie("electsys.sjtu.edu.cn", "ASP.NET_SessionId")
        if sessionID != "":
            print "Get Score Info, Waiting for minutes..."
            postData = {'cookie':'ASP.NET_SessionId=' + sessionID}
            result = requests.post(crossyWebPrefix + "/api/1/elect/info", postData)

    print "Deal With Score Info, Waiting for minutes..."
    scoreData = json.loads(result.text, encoding='utf-8')
    scoreInfo = dealWithScore(scoreData['courses'])
    for i in scoreInfo:
        print i
        for j in scoreInfo[i]:
            j.printCourse()

#---------------------------------------------------------------------

#------------------------deal with books query------------------------
def booksQueryFunc():
    print "Querying Books..."
#---------------------------------------------------------------------

#----------------------- deal with bus query--------------------------
def busQueryFunc():
    print "Querying Bus Schedule..."

    print "Get Bus Schedule, Waiting for minutes..."
    result = requests.get(crossyWebPrefix + "/api/1/bus/weekday/xuhui2minhang")
    busDataXuhui2Minhang = json.loads(result.text, encoding='utf-8')
    print busDataXuhui2Minhang
#---------------------------------------------------------------------

#------------------------deal with ecard query------------------------
def ecardQueryFunc():
    print "Querying E-Card Info..."
#---------------------------------------------------------------------

def main():
    print "---------Crossy Server---------"
    queryType = "busQuery"

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
