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
import time
import serial

crossyWebPrefix = "http://tq5124.dy.tongqu.me/crossy"
TIMEFORMAT = '%Y%m%d'

ser = serial.Serial("COM4", 115200)

#------------------------course information--------------------------
class courses:
    name = ""
    credit = 0
    score = 0
    def __init__(self, n, c, s):
        self.credit = 'credits: ' + c + ' ; '
        self.name= 'name: ' + n + ' ; '
        self.score = 'score: ' + s
    def printCourse(self):
        print self.name + self.credit + self.score
    def coursesInString(self):
        return self.name + self.credit + self.score
#--------------------------------------------------------------------

#-----------------------book information-----------------------------
class libBooks:
    name = ""
    author = ""
    press = ""
    def __init__(self, n, a, p):
        self.name = "name: " + n
        self.author = "author: " + a
        self.press = "press: " + p
    def printBooks(self):
        print self.name + " ; " + self.author + " ; " + self.press
#------------------------------------------------------------------

#------------------------------get cookies---------------------------
def getLocalCookie(url, key, filename):
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
            with open(filename, 'w') as outFile:
                outFile.write(ret[1].encode('gbk'))
    conn.close()
    file = open(filename, 'r')
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

def dealWithGPA(data):
    gpa = ""
    for i in data:
        if i.get('shortName', 'none') == "all":
            gpa = 'gpa: ' + str(i['gpa']) + ' ; credits:' + str(i['credits'])
    return gpa

def scoreQueryFunc():
    print "Query Score..."

    sessionID = getLocalCookie("electsys.sjtu.edu.cn", "ASP.NET_SessionId", "cookie.txt")
    postData = {"cookie":"ASP.NET_SessionId=" + sessionID}
    print "Get Score Info, Waiting for minutes..."
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

        sessionID = getLocalCookie("electsys.sjtu.edu.cn", "ASP.NET_SessionId", "cookie.txt")
        if sessionID != "":
            postData = {'cookie':'ASP.NET_SessionId=' + sessionID}
            print "Get Score Info, Waiting for minutes..."
            result = requests.post(crossyWebPrefix + "/api/1/elect/info", postData)

    print "Deal With Score Info, Waiting for minutes..."
    scoreData = json.loads(result.text, encoding='utf-8')
    scoreInfo = dealWithScore(scoreData['courses'])
    gpaInfo = dealWithGPA(scoreData['gpa'])

    for i in scoreInfo:
        print i
        for j in scoreInfo[i]:
            j.printCourse()
    print gpaInfo


#---------------------------------------------------------------------

#------------------------deal with books query------------------------
def dealWithBooks(data):
    books = []
    for i in data:
        bks = libBooks(i['title'], i['author'], i['others'])
        books.append(bks)
    return books

def booksQueryFunc():
    print "Querying Books..."
    print "Please input the book's name"
    name = raw_input();

    postData = {'q':name}
    print "Get Books Info, Waiting for minutes..."
    result = requests.post(crossyWebPrefix + "/api/1/lib/search", postData)

    print "Deal With Books Info, Waiting for minutes..."
    bookData = json.loads(result.text, encoding='utf-8')
    booksInfo = dealWithBooks(bookData)

    for i in booksInfo:
        i.printBooks()
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
#NOTE: THE HISTORY OF ECARD IS UNAVAILABLE
def ecardQueryFunc():
    print "Querying E-Card Info..."
    currentTime = time.strftime(TIMEFORMAT, time.localtime())

    sessionID = getLocalCookie("ecard.sjtu.edu.cn", "JSESSIONID", "ecardSession.txt")
    postDataInfo = {"cookie":"JSESSIONID=" + sessionID}
    #postDataHistory = {"cookie":"JSESSIONID=" + sessionID, "startDate":"20150501", "endDate": currentTime}
    print "Get E-Card Info, Waiting for minutes..."
    resultInfo = requests.post(crossyWebPrefix + "/api/1/ecard/info", postDataInfo)
    #resultHistory = requests.post(crossyWebPrefix + "/api/1/ecard/history", postDataHistory)

    #if resultInfo.status_code == 400 or resultHistory.status_code == 400:
    if resultInfo.status_code == 400:
        print "please login"
        os.system("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe")
        webbrowser.open("http://ecard.sjtu.edu.cn/shjdportalHome.jsp")
        print("Input \"login complete\" when you have logined")
        while (True):
            a = raw_input()
            if a == "login complete":
                break

        sessionID = getLocalCookie("ecard.sjtu.edu.cn", "JSESSIONID", "ecardSession.txt")
        if sessionID != "":
            postDataInfo = {"cookie":"JSESSIONID=" + sessionID}
            #postDataHistory = {"cookie":"JSESSIONID=" + sessionID, "startDate":"20150501", "endDate": currentTime}
            print "Get E-Card Info, Waiting for minutes..."
            resultInfo = requests.post(crossyWebPrefix + "/api/1/ecard/info", postDataInfo)
            #resultHistory = requests.post(crossyWebPrefix + "/api/1/ecard/history", postDataHistory)

    print "Deal With E-Card Info, Waiting for minutes..."
    ecardInfo = json.loads(resultInfo.text, encoding='utf-8')
    print ecardInfo

#---------------------------------------------------------------------

#--------------------------------------serial-------------------------
def readStringFromPort(ser):
    stringRead = ""
    charRead = ser.read();
    while (charRead != '\n'):
        stringRead += charRead
        charRead = ser.read()
    return stringRead




def main():
    print "---------Crossy Server---------"
    print "---------Connecting to  CORTEX M3 S700 Board--------"

    if ser.isOpen():
        print "---------Successful Connection!---------------------"

    while (1):
        queryType = readStringFromPort(ser)
        if queryType == "scoreQuery":
            scoreQueryFunc()
        elif queryType == "booksQuery":
            booksQueryFunc()
        elif queryType == "busQuery":
            busQueryFunc()
        elif queryType == "ecardQuery":
            ecardQueryFunc();
        else:
            print "Unavailable Type. Please Check."
            break


if __name__ == '__main__':
    main()
