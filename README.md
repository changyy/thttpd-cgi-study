# Workspace #

```
$ mkdir -p workspace/html/cgi-bin workspace/run workspace/log workspace/sbin
$ cp ./thttpd workspace/sbin
$ workspace/
├── html
│   └── cgi-bin
├── log
│   └── thttpd.log
├── run
│   └── thttpd.pid
├── sbin
│   └── thttpd
└── thttpd.conf

5 directories, 4 files

$ ./workspace/sbin/thttpd -C ./workspace/thttpd.conf
```

---

# Test CGI #

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cp study-curl.cgi ../workspace/html/cgi-bin
```

---

```
% curl http://localhost:8000/cgi-bin/study-curl.cgi
<html><body><pre>
== Server Info - Begin ==
PATH=/usr/local/bin:/usr/ucb:/bin:/usr/bin
LD_LIBRARY_PATH=(null)
SERVER_SOFTWARE=thttpd/2.29 23May2018
SERVER_NAME=localhost
GATEWAY_INTERFACE=CGI/1.1
SERVER_PROTOCOL=HTTP/1.1
SERVER_PORT=8000
REQUEST_METHOD=GET
PATH_INFO=(null)
PATH_TRANSLATED=(null)
SCRIPT_NAME=/cgi-bin/study-curl.cgi
QUERY_STRING=(null)
REMOTE_ADDR=::1
HTTP_REFERER=(null)
HTTP_USER_AGENT=curl/7.68.0
HTTP_ACCEPT=*/*
HTTP_ACCEPT_ENCODING=(null)
HTTP_ACCEPT_LANGUAGE=(null)
HTTP_COOKIE=(null)
CONTENT_TYPE=(null)
HTTP_HOST=localhost
CONTENT_LENGTH=(null)
REMOTE_USER=(null)
AUTH_TYPE=(null)
TZ=(null)
CGI_PATTERN=cgi-bin/*
-- Server Info - End --


Using libcurl fire a request: https://dns.google/resolve?name=google.com

== Response code: 200 ==
== Response Header - Begin ==
HTTP/2 200 
strict-transport-security: max-age=31536000; includeSubDomains; preload
access-control-allow-origin: *
date: Thu, 06 Feb 2020 10:22:13 GMT
expires: Thu, 06 Feb 2020 10:22:13 GMT
cache-control: private, max-age=297
content-type: application/x-javascript; charset=UTF-8
server: HTTP server (unknown)
x-xss-protection: 0
x-frame-options: SAMEORIGIN
alt-svc: quic=":443"; ma=2592000; v="46,43",h3-Q050=":443"; ma=2592000,h3-Q049=":443"; ma=2592000,h3-Q048=":443"; ma=2592000,h3-Q046=":443"; ma=2592000,h3-Q043=":443"; ma=2592000
accept-ranges: none
vary: Accept-Encoding


-- Response Header - End --
== Response Body - Begin == 
{"Status": 0,"TC": false,"RD": true,"RA": true,"AD": false,"CD": false,"Question":[ {"name": "google.com.","type": 1}],"Answer":[ {"name": "google.com.","type": 1,"TTL": 297,"data": "172.217.27.142"}]}
-- Response Body - End --
</pre></body></html>
```
