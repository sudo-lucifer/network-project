import http.client
import socket as sk
from time import sleep

#=======================================

myhost = 'localhost'
myport = 28581

#=======================================

def testOK1():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        s.sendall(b'HEAD ')
        sleep(1)
        s.sendall(b'/ HTTP/1.1\r')
        sleep(0.5)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th\r\nConnection: close\r\n')
        sleep(2)
        s.sendall(b'Abc: yay\r\nWow: hurray\r\n')
        s.sendall(b'\r\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")



def testOK2():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        s.sendall(b'G')
        sleep(1)
        s.sendall(b'E')
        sleep(1)
        s.sendall(b'T ')
        sleep(1.5)
        s.sendall(b'/ HTTP/1.1\r')
        sleep(5)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th')
        sleep(0.1)
        s.sendall(b'\r\n')
        sleep(1)
        s.sendall(b'\r\n')

        while data := s.recv(1024):
            print(str(data, 'UTF-8') )
    print("=====================================")


def testOK3():
    import sys
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        info = b'HEAD / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n'
        
        for j in [i.to_bytes(1, sys.byteorder) for i in info]:
            s.sendall(j)
            sleep(0.02)
        
        while data := s.recv(1024):
            print(str(data, 'UTF-8') )
    print("=====================================")


def testOK4():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        
        while data := s.recv(1024):
            print( str(data , 'UTF-8' ) )
    print("=====================================")

def testOK5():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        s.sendall(b'HEAD / HTTP/2020.2\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        
        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")


def testOK6():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        s.sendall(b'p')
        sleep(1)
        s.sendall(b'u')
        sleep(1)
        s.sendall(b't ')
        sleep(1.5)
        s.sendall(b'/ HTTP/1.1\r')
        sleep(5)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th')
        sleep(0.1)
        s.sendall(b'\r\n')
        sleep(1)
        s.sendall(b'\r\n')
        #s.sendall(b'<html>\n<head><title>301 Moved Permanently</title></head>\n<body>\n<center><h1>301 Moved Permanently</h1></center>\n<hr><center>nginx/1.19.10</center>\n</body>\n</html>')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")

def testOK7():
    print("=====================================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        
        sleep(5)
        s.sendall(b'HEAD /lmao.abc HTTP/1.1\r')
        sleep(1)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th')
        sleep(0.1)
        s.sendall(b'\r\n')
        sleep(1)
        s.sendall(b'\r\n')
        #s.sendall(b'<html>\n<head><title>301 Moved Permanently</title></head>\n<body>\n<center><h1>301 Moved Permanently</h1></center>\n<hr><center>nginx/1.19.10</center>\n</body>\n</html>')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")

'''
testOK1()
testOK2()
testOK3()
testOK4()
testOK5()
testOK6()
testOK7()
'''
def testBad():
    print("==================501===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(1)
        s.sendall(b'POST / HTTP/1.1\r')
        sleep(0.5)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th\r\nConnection: close\r\n')
        sleep(2)
        s.sendall(b'Abc: yay\r\nWow: hurray\r\n')
        s.sendall(b'\r\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")

def testBAD1():
    print("==================400===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(1)
        s.sendall(b'/ HTTP/1.1\r')
        sleep(0.5)
        s.sendall(b'\n')
        s.sendall(b'Host: cs.muic.mahidol.ac.th\r\nConnection: close\r\n')
        sleep(2)
        s.sendall(b'Abc: yay\r\nWow: hurray\r\n')
        s.sendall(b'\r\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")

def testBAD2():
    print("==================400===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(1)
        s.sendall(b'get / \r')
        sleep(0.5)
        s.sendall(b'\n')
        s.sendall(b'\r\n')
        

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")


def testBAD3():
    print("===============400===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")


def testBAD4():
    print("================400===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        s.sendall(b'Host:aadsfadsfasdfasdf')
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'Host:aadsfadsfasdfasdf')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")


def testBAD5():
    import sys
    print("=================200===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        info = b'HEAD / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n'
        
        for j in [i.to_bytes(1, sys.byteorder) for i in info]:
            s.sendall(j)
            sleep(0.02)
        
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        
        while data := s.recv(1024):
            print(str(data, 'UTF-8') )
    print("=====================================")


def testBAD6():
    
    print("================400=================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        s.sendall(b'a a a\r\n')
        sleep(1)
        s.sendall(b'hello: asdfadsfasdfasdf\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        
        while data := s.recv(1024):
            print(str(data, 'UTF-8') )
    print("=====================================")


def testBAD7():
    print("===============505=================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        #s.sendall(b'GET / HTTP/1.1\r\nHost: cs.muic.mahidol.ac.th\r\nConnection: close\r\n\r\n')
        s.sendall(b'lmao.txt /abc adfgsdg\r\n')
        sleep(1)
        s.sendall(b'hello: asdfadsfasdfasdf\r')
        sleep(0.5)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r')
        sleep(0.5)
        s.sendall(b'\n')
        
        while data := s.recv(1024):
            print(str(data, 'UTF-8') )
    print("=====================================")

# 505 HTTP version not supported
def myTest():
    print("================200==================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(5)
        s.sendall(b'HEAD / HTTP/1.1')
        sleep(1)
        s.sendall(b'\r')
        sleep(1)
        s.sendall(b'\n')
        sleep(1)
        s.sendall(b'\r\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")


# 200 OK
def myTest2():
    print("================200===================")
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        # (host: str, port: int)
        s.connect((myhost, myport))

        sleep(1)
        s.sendall(b'HEAD cat.jpg HTTP/1.1\r')
        sleep(1)
        s.sendall(b'\n\r\n')

        while data := s.recv(1024):
            print( str(data, 'UTF-8') )
    print("=====================================")

# testBAD1()
# testBAD2()
# testBAD3()
# testBAD4()
# testBAD5()
# testBAD6()
# testBAD7()
# testOK1()
# testBAD1()
# testOK2()
# myTest()
# myTest2()
# testBAD1()
# myTest()
# testOK1()
# testBAD5()
print("=====================================")
with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:

    myhost = 'localhost'
    myport = 28581
    #myport = 20163
    # (host: str, port: int)
    
    s.connect((myhost, myport))

    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'HEAD / Http/1.1\r\nConnection: keep-alive\r\n\r\n')
    s.sendall(b'abc\r\n\r\n')
    # s.sendall(b'HEAD / Http/1.1\r\nConnection: close\r\n\r\n')
    while data := s.recv(1024):
        print( str(data, 'UTF-8') )
print("=====================================")
