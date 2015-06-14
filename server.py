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
import io
from bs4 import BeautifulSoup

crossyWebPrefix = "http://tq5124.dy.tongqu.me/crossy"
TIMEFORMAT = '%Y%m%d'

ser = serial.Serial("COM4", 115200)
#------------------------course information--------------------------
class courses:
    code = ""
    credit = 0
    score = 0
    def __init__(self, n, c, s):
        self.credit = 'credits: ' + c.replace(" ", "") + ' ; '
        self.code= 'code: ' + n.replace(" ", "") + ' ; '
        self.score = 'score: ' + s.replace(" ", "")
    def printCourse(self):
        print self.code + self.credit + self.score
    def coursesInString(self):
        return self.code + self.credit + self.score
#--------------------------------------------------------------------

#-----------------------book information-----------------------------
class libBooks:
    name = ""
    author = ""
    def __init__(self, n, a):
        self.name = "name: " + n.replace(" ", "")
        self.author = "author: " + a.replace(" ", "")
        self.author = self.author.replace(",", " ")
    def printBooks(self):
        print self.name + " ; " + self.author
    def booksInString(self):
        return self.name + self.author
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
        crs = courses(i['code'], i['credit'], i['score'])
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
            gpa = str((float(int(i['gpa'] * 1000)))/1000)
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
    gpaInfo =  gpaInfo.encode()
    ser.write(gpaInfo + '@')
    print "gpa transmition completed"

    while (1):
        chosenSemester = readStringFromPort(ser)
        if chosenSemester == "return" or chosenSemester == "Initial Done" or chosenSemester == "\x00Initial Done":
            print "score query ended"
            break;
        else:
            for i in scoreInfo[chosenSemester]:
                outputData = i.coursesInString()
                outputData = outputData.encode()
                ser.write(outputData + '@')
                for i in range(10000000):
                    None
                #need a delay, otherwise the data will be lost
            print chosenSemester + " data transmission completed"
            ser.write('#' + '@')

#---------------------------------------------------------------------

#------------------------deal with books query------------------------
def dealWithBooks(data):
    books = []
    for i in data:
        name = i['title']
        tmpname = ""
        if (name.find('=') != -1):
            tmp = name.split("=")
            if (chineseOrNot(tmp[0])):
                tmpname = tmp[1]
            else:
                tmpname = tmp[0]
        else:
            if (chineseOrNot(name)):
                continue
            else:
                tmpname = name

        author = i['author']
        tmpauthor = ""
        right = 0
        while (author.find("(", right) != -1):
            left = author.find("(", right) + right
            right = author.find(")", right) + right
            if (not chineseOrNot(author[left + 1:right])):
                tmpauthor += author[left + 1:right] + "    "
        if (author.find("(") == -1):
            if (not chineseOrNot(author)):
                tmpauthor = author
        if tmpauthor == "":
            continue

        bks = libBooks(tmpname, tmpauthor)
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

    while (1):
        chosenSemester = readStringFromPort(ser)
        if chosenSemester == "return" or chosenSemester == "Initial Done" or chosenSemester == "\x00Initial Done":
            print "books query ended"
            break;
        else:
            for i in booksInfo:
                outputData = i.booksInString()
                outputData = outputData.encode()
                print outputData
                ser.write(outputData + '@')
                for i in range(10000000):
                    None
                    #need a delay, otherwise the data will be lost
            ser.write('#' + '@')
            print "books about " + name + " transmission completed"
#---------------------------------------------------------------------

#----------------------- deal with bus query--------------------------
def busQueryFunc():
    print "Querying Bus Schedule..."

    print "Get Bus Schedule, Waiting for minutes..."
    result = requests.get(crossyWebPrefix + "/api/1/bus/weekday/xuhui2minhang")
    busDataXuhui2Minhang = json.loads(result.text, encoding='utf-8')
    print "Deal With Bus Schedule, Waiting for minutes..."
    while (1):
        chosenOperation = readStringFromPort(ser)
        if chosenOperation == "return" or chosenOperation == "Initial Done" or chosenOperation == "\x00Initial Done":
            print "bus query ended"
            break;
        elif chosenOperation == "minhang2xuhui":
            print "minhang to xuhui schedule transmission completed"
        elif chosenOperation == "schoolBus":
            print "show school bus picture"
#---------------------------------------------------------------------

#------------------------deal with ecard query------------------------
#NOTE: THE HISTORY OF ECARD IS UNAVAILABLE
def ecardQueryFunc():
    print "Querying E-Card Info..."

    sessionID = getLocalCookie("ecard.sjtu.edu.cn", "JSESSIONID", "ecardSession.txt")
    postDataInfo = {"JSESSIONID":sessionID}
    resultInfo = requests.get("http://ecard.sjtu.edu.cn/accountcardUser.action", cookies = postDataInfo)
    html = BeautifulSoup(resultInfo.text)
    remaining = html.find_all("td", "neiwen")[44]
    outputData = "remaining: " + remaining.get_text()[0:4].encode()
    ser.write(outputData + "@")
    print "remaining data transmission completed"

    while(1):
        chosenOperation = readStringFromPort(ser)
        if chosenOperation == "return" or chosenOperation == "Initial Done" or chosenOperation == "\x00Initial Done":
            print "ecard query ended"
            break;

#---------------------------------------------------------------------

#--------------------------------------serial-------------------------
def readStringFromPort(ser):
    stringRead = ""
    charRead = ser.read();
    while (charRead != '\n'):
        stringRead += charRead
        charRead = ser.read()
    return stringRead

#-------------------------------------other---------------------------
def boolToString(data):
    if (data == True):
        return "true"
    else: return "false"

def chineseOrNot(data):
    for i in data:
        if (i >= u'\u4e00' and i <= u'\u9fa5'):
            return True
    return False


def main():
    print "---------Crossy Server---------"
    print "---------Connecting to  CORTEX M3 S700 Board--------"

    if ser.isOpen():
        print "---------Successful Connection!---------------------"

    while (ser.isOpen()):
        queryType = readStringFromPort(ser)
        #print queryType
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
            ser.close()
            break


if __name__ == '__main__':
    main()
